/*
    File: kernel-rubric.C

    Author: R. Bettati, Dilma Da Silva
            Department of Computer Science
            Texas A&M University

    This file has the main entry point to the operating system.

*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define EXPECT_TRUE(X) if (!(X)) { assert(false);}
#define EXPECT_FALSE(X) if (X) { assert(false);}

#define GB * (0x1 << 30)
#define MB * (0x1 << 20)
#define KB * (0x1 << 10)
#define KERNEL_POOL_START_FRAME ((2 MB) / Machine::PAGE_SIZE)
#define KERNEL_POOL_SIZE ((2 MB) / Machine::PAGE_SIZE)
#define PROCESS_POOL_START_FRAME ((4 MB) / Machine::PAGE_SIZE)
#define PROCESS_POOL_SIZE ((28 MB) / Machine::PAGE_SIZE)
/* definition of the kernel and process memory pools */

#define MEM_HOLE_START_FRAME ((15 MB) / Machine::PAGE_SIZE)
#define MEM_HOLE_SIZE ((1 MB) / Machine::PAGE_SIZE)
/* we have a 1 MB hole in physical memory starting at address 15 MB */

#define FAULT_ADDR (4 MB)
/* used in the code later as address referenced to cause page faults. */
#define NACCESS ((1 MB) / 4)
/* NACCESS integer access (i.e. 4 bytes in each access) are made starting at address FAULT_ADDR */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "machine.H"        /* LOW-LEVEL STUFF */
#include "console.H"
#include "gdt.H"
#include "idt.H"            /* LOW-LEVEL EXCEPTION MGMT. */
#include "irq.H"
#include "exceptions.H"
#include "interrupts.H"

#include "simple_keyboard.H" /* SIMPLE KB DRIVER */
#include "simple_timer.H"   /* SIMPLE TIMER MANAGEMENT */

#include "page_table.H"
#include "paging_low.H"

#include "vm_pool.H"

/*--------------------------------------------------------------------------*/
/* FORWARD REFERENCES FOR TEST CODE */
/*--------------------------------------------------------------------------*/

void TestPassed();
void TestFailed();

void GeneratePageTableMemoryReferences(unsigned long start_address, int n_references);
void GenerateVMPoolMemoryReferences(VMPool *pool, int size1, int size2);

void output_console_file(const char* _string);

/*--------------------------------------------------------------------------*/
/* MEMORY ALLOCATION */
/*--------------------------------------------------------------------------*/

VMPool *current_pool;

typedef unsigned int size_t;

//replace the operator "new"
void * operator new (size_t size) {
  unsigned long a = current_pool->allocate((unsigned long)size);
  return (void *)a;
}

//replace the operator "new[]"
void * operator new[] (size_t size) {
  unsigned long a = current_pool->allocate((unsigned long)size);
  return (void *)a;
}

//replace the operator "delete"
void operator delete (void * p) {
  current_pool->release((unsigned long)p);
}

//replace the operator "delete[]"
void operator delete[] (void * p) {
  current_pool->release((unsigned long)p);
}

/*--------------------------------------------------------------------------*/
/* EXCEPTION HANDLERS */
/*--------------------------------------------------------------------------*/

/* -- EXAMPLE OF THE DIVISION-BY-ZERO HANDLER */

void dbz_handler(REGS * r) {
  Console::puts("DIVISION BY ZERO\n");
  for(;;);
}


/*--------------------------------------------------------------------------*/
/* MAIN ENTRY INTO THE OS */
/*--------------------------------------------------------------------------*/

int main() {

   GDT::init();
    Console::init();
    IDT::init();
    ExceptionHandler::init_dispatcher();
    IRQ::init();
    InterruptHandler::init_dispatcher();


    /* -- EXAMPLE OF AN EXCEPTION HANDLER -- */
    
    class DBZ_Handler : public ExceptionHandler {
      /* We derive Division-by-Zero handler from ExceptionHandler 
         and overload the method handle_exception. */
    public:
        virtual void handle_exception(REGS * _regs) {
            Console::puts("DIVISION BY ZERO!\n");
            for(;;);
        }
    } dbz_handler;
    
    /* Register the DBZ handler for exception no.0 
       with the exception dispatcher. */
    ExceptionHandler::register_handler(0, &dbz_handler);
    

    /* -- INITIALIZE THE TIMER (we use a very simple timer).-- */
    
    SimpleTimer timer(100); /* timer ticks every 10ms. */
    
    /* ---- Register timer handler for interrupt no.0 
            with the interrupt dispatcher. */
    InterruptHandler::register_handler(0, &timer);
    
    /* NOTE: The timer chip starts periodically firing as
     soon as we enable interrupts.
     It is important to install a timer handler, as we
     would get a lot of uncaptured interrupts otherwise. */
    
    /* -- INSTALL KEYBOARD HANDLER -- */
    SimpleKeyboard::init();

    Console::puts("after installing keyboard handler\n");

    /* -- ENABLE INTERRUPTS -- */
    
    Machine::enable_interrupts();

    /* -- INITIALIZE FRAME POOLS -- */

    ContFramePool kernel_mem_pool(KERNEL_POOL_START_FRAME,
                                  KERNEL_POOL_SIZE,
                                  0,
				                  0);

    unsigned long n_info_frames = 
      ContFramePool::needed_info_frames(PROCESS_POOL_SIZE);

    unsigned long process_mem_pool_info_frame = 
    kernel_mem_pool.get_frames(n_info_frames);

    ContFramePool process_mem_pool(PROCESS_POOL_START_FRAME,
                                   PROCESS_POOL_SIZE,
                                   process_mem_pool_info_frame,
				                   n_info_frames);

    /* Take care of the hole in the memory. */
    process_mem_pool.mark_inaccessible(MEM_HOLE_START_FRAME, MEM_HOLE_SIZE);

    /* -- INITIALIZE MEMORY (PAGING) -- */

    /* ---- INSTALL PAGE FAULT HANDLER -- */

    class PageFault_Handler : public ExceptionHandler {
      /* We derive the page fault handler from ExceptionHandler 
	 and overload the method handle_exception. */
      public:
      virtual void handle_exception(REGS * _regs) {
        PageTable::handle_fault(_regs);
      }
    } pagefault_handler;

    /* ---- Register the page fault handler for exception no. 14
            with the exception dispatcher. */
    ExceptionHandler::register_handler(14, &pagefault_handler);

    /* ---- INITIALIZE THE PAGE TABLE -- */

    PageTable::init_paging(&kernel_mem_pool,
                           &process_mem_pool,
                           4 MB);

    PageTable pt1;

    pt1.load();

    PageTable::enable_paging();

    /* Note about messages on the screen:
    **
    ** In the provided code, the output you had in P3 
    ** EXCEPTION DISPATCHER: exc_no = 14 has been supressed.
    ** The output in exceptions.C was commented out (lines 138-140)
    ** If you think it is helpful to have the messages indicating that
    ** handle_fault is being triggered, you can uncomment them.
    */

    /* -- INITIALIZE THE TWO VIRTUAL MEMORY PAGE POOLS -- */

    /* -- MOST OF WHAT WE NEED IS SETUP. THE KERNEL CAN START. */

    Console::puts("Hello World!\n");

    /* WE TEST JUST THE VM POOLS */



    output_console_file("GRADING - Constructor ran without breaking the system.\n");
    output_console_file("GRADING - It has at least 10 points for compiling and booting.\n");


    /* -- CREATE THE VM POOLS. */

    VMPool small_pool(512 MB, 8 KB, &process_mem_pool, &pt1);
    /* -- INITIAL TESTING, a few simple calls */
    current_pool = &small_pool;
    EXPECT_FALSE(small_pool.is_legitimate(300 MB));
    EXPECT_FALSE(small_pool.is_legitimate( (512 MB) + (4 KB)));
    int *array1 = new int[1024];
    EXPECT_TRUE(small_pool.is_legitimate((unsigned long) array1));
    EXPECT_TRUE(small_pool.is_legitimate((unsigned long) &array1[1023]));
    EXPECT_FALSE(small_pool.is_legitimate((unsigned long)array1 + 8 KB));
    int *array2 = new int[2048]; // not enough space
    EXPECT_TRUE(array2 == 0);
    delete array1;
    EXPECT_FALSE(small_pool.is_legitimate((unsigned long) array1));
    array1 = new int[1024]; // we have to have space for this now
    EXPECT_TRUE(array1 != 0);

    Console::puts("Success in simple tests with small_pool.\n");
    output_console_file("GRADING - It succeded with small_pool tests.\n");
    output_console_file("GRADING - It has at least 25 points out of 50.\n");

    VMPool code_pool(512 MB, 256 MB, &process_mem_pool, &pt1);
    VMPool heap_pool(1 GB, 256 MB, &process_mem_pool, &pt1);
    
    /* -- NOW THE POOLS HAVE BEEN CREATED. */

    Console::puts("VM Pools successfully created!\n");

    /* -- EXTENSIVE TESTING:
    **    GENERATE MEMORY REFERENCES TO THE VM POOLS */

    Console::puts("I am starting with an extensive test\n");
    Console::puts("of the VM Pool memory allocator.\n");
    Console::puts("Please be patient...\n");
    Console::puts("Testing the memory allocation on code_pool...\n");
    GenerateVMPoolMemoryReferences(&code_pool, 50, 100);
    Console::puts("Testing the memory allocation on heap_pool...\n");
    GenerateVMPoolMemoryReferences(&heap_pool, 50, 100);

    TestPassed();

    output_console_file("GRADING - small_pool and memory reference tests.\n");
    output_console_file("GRADING - Submission has at least 35 points out of 50.\n");
    
    const unsigned long base = 800 MB;
    unsigned long space_available = 64 KB;
    VMPool v(base, 64 KB, &process_mem_pool, &pt1);
    unsigned long addr1 = v.allocate(32 KB);
    space_available -= 32 KB;
    unsigned long space_used_by_region_list = addr1 - base;
    space_available -= space_used_by_region_list;
    if (space_available < 16 KB) {
        /* implementation is wasting memory; it used more than 18 KB to
        ** store its array? 
        */
        output_console_file("GRADING space_available is too small - It did not go beyond initial tests.\n");
        output_console_file("GRADING - Grade is 35 out of 50\n");
    } else {
        unsigned long addr2 = v.allocate(12 KB);
        space_available -= 12 KB;
        unsigned long addr3 = v.allocate(space_available);
        if (addr2 == 0 || addr3 == 0) {
            output_console_file("GRADING allocation failed - It did not go beyond initial tests.\n");
            output_console_file("GRADING - Grade is 35 out of 50\n"); 
        } else {
            /* now we will release memory and try to allocate it
            ** again. Implementations that always allocate at the
            ** end of their allocations (not noticing 'holes' created
            ** by releases) are likely to fail.
            */
            v.release(addr2);
            unsigned long addr4 = v.allocate(10 KB);
            if ( (addr4 >= (addr1 + 32 KB) )
                      && addr4 < addr3) {
                /* at least it found the gap created by the relase of addr2
                */
                output_console_file("GRADING - It reused released memory.\n");
                output_console_file("GRADING - Grade is  at least 45 out of 50\n");

                /* This catches implementations that allow their list of
                ** allocated regions to go beyond the space they reserved for
                ** it */
                VMPool v2(2 GB, 512 MB, &process_mem_pool, &pt1);
                unsigned long address = v2.allocate(4 KB);
                unsigned long internal_space = address - (2 GB);
                unsigned long how_many_to_create;
                if (internal_space == 0) {
                    // implementation is likely to be using data member for array, 
                    // so limited in the number of regions.
                    how_many_to_create = 100;
                } else {
                    // most students will have a region represented through a structure
                    // with two numbers
                    how_many_to_create = internal_space/8 + 20;
                }
                debug_out_E9_msg_value("how_many_to_create is ", how_many_to_create);
                for (unsigned long i = 0; i < how_many_to_create && address != 0; i++) {
                    address = v2.allocate(4 KB); /* there is space for the 
                                                 ** request, but eventually
                                                 ** not enough space in the array
                                                 ** of lists used in the implementation */
                }
                if (address == 0) {
                    // implementation has bounded array of regions
                    output_console_file("GRADING - Grade is  50 out of 50\n");
                } else {
                    output_console_file("GRADING - It went behyond space for array\n");
                    output_console_file("GRADING - Grade is 45 out of 50\n");                    
                }

            } else {
                debug_out_E9_msg_value("addr1", addr1);
                debug_out_E9_msg_value("addr3", addr3);
                debug_out_E9_msg_value("addr4", addr4);
                output_console_file("GRADING add4 out of expected range - It did not go beyond initial tests.\n");
                output_console_file("GRADING - Grade is 35 out of 50\n"); 
            } 
        }
    }
    Console::puts("YOU CAN SAFELY TURN OFF THE MACHINE NOW.\n");
    for(;;);
}

void GeneratePageTableMemoryReferences(unsigned long start_address, int n_references) {
  int *foo = (int *) start_address;
  
  for (int i=0; i<n_references; i++) {
    foo[i] = i;
  }
  
  Console::puts("DONE WRITING TO MEMORY. Now testing...\n");

  for (int i=0; i<n_references; i++) {
    if(foo[i] != i) {
      TestFailed();
    }
  }
}

void GenerateVMPoolMemoryReferences(VMPool *pool, int size1, int size2) {
   current_pool = pool;
   for(int i=1; i<size1; i++) {
      int *arr = new int[size2 * i];
      if(pool->is_legitimate((unsigned long)arr) == false) {
         TestFailed();
      }
      for(int j=0; j<size2*i; j++) {
         arr[j] = j;
      }
      for(int j=size2*i - 1; j>=0; j--) {
         if(arr[j] != j) {
            TestFailed();
         }
      }
      delete arr;
   }
}

void TestFailed() {
   Console::puts("Test Failed\n");
   Console::puts("YOU CAN TURN OFF THE MACHINE NOW.\n");
   for(;;);
}

void TestPassed() {
   Console::puts("Test Passed! Congratulations!\n");
   //Console::puts("YOU CAN SAFELY TURN OFF THE MACHINE NOW.\n");
   //for(;;);
}

void output_console_file(const char* _string) {
    Console::puts(_string);
    debug_out_E9(_string);
}
