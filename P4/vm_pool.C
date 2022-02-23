/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/
int VMPool::count_regions = 0;

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    // replace the assertion with your constructor code

    // You need to register this VMPool with the page table.
    // You can do that by invoking method PageTable::register_pool
    // The page_table object file that we provide (named page_table_provided.o)
    // contains this method.

    base_address = _base_address;
    size = _size;
    frame_pool = _frame_pool;
    page_table = _page_table;
    max = base_address + size;
    count_regions = 0;
     
    // USE FIRST PAGE FOR THE STORAGE - not doing that now
    //p_store = (pool_storage*)(base_address)

    page_table->register_pool(this);

    Console::puts("Constructed VMPool object.\n");

}

void update_storage(pool_storage p[], unsigned long base, unsigned long s, int i){							
	    p[i].addr = base;
	    p[i].size = s;
	    p[i].allocated = true;
	    return;
}


unsigned long VMPool::allocate(unsigned long _size) {


     if(count_regions >= 20)
		return 0;

    // replace the assertion with your code
    // WHEN THERE IS NO REGIONS
    if(count_regions == 0){        	
          update_storage(p_store, base_address, _size, count_regions);   
	      count_regions++;
          return base_address; 
    }
    else{ // WHEN THERE ARE REGIONS 
	
    	//if there is space in the end
        if((p_store[count_regions-1].addr + _size) < max){
            update_storage(p_store, (p_store[count_regions-1].addr + p_store[count_regions-1].size), _size, count_regions);
	        count_regions++;
            return (p_store[count_regions-1].addr+p_store[count_regions-1].size );

        }else {
            // there is not space in the end. There may be space between elements in the array
            // iterate through the array, checking if there is space beween k and k+1
            for (unsigned int k = 0; k < count_regions - 1; k++) {		    
                unsigned long end_of_regionk = p_store[k].addr + p_store[k].size;
                if ( (end_of_regionk + _size) < p_store[k+1].addr) {		    
                    // found space starting at address end_of_regionk.
		            update_storage(p_store, end_of_regionk, _size, count_regions);
		            count_regions++;
            	    return (p_store[count_regions-1].addr + p_store[count_regions-1].size);			    
                }
            }// END ELSE
	    return 0;
        }// END ELSE
    }
    Console::puts("Allocated region of memory.\n");
}

void VMPool::release(unsigned long _start_address) {
    // Notice that the regions being released may be in
    // the page tables. We want to unmap all pages in the
    // region being released, remove them from the page table.
    // To unmap virtual page addr you can use
    // method PageTable::free_page:
    //      page_table->free_page(addr)
    // The page_table object file that we provide (named page_table_provided.o)
    // contains this method.
    // replace the assertion with your code
 
    int index = 0;

    // Find the correct region
    while(index <= count_regions){
        if(p_store[index].addr == _start_address)
            break;
	      index++;
    }

    for(int i = index; i < count_regions-1; i++)
		p_store[i] = p_store[i+1];
	
    page_table->free_page(_start_address);
     
    count_regions--;  	
    
    Console::puts("Released region of memory.\n");
  
    /*   
    pool_storage new_pool[20];	
    int i = 0;
    // Copy values
    while(i < count_regions -1){

       if(i != index){
	      	new_pool[i] = p_store[i];
       		i++;
       }
    }

    // Paste values
    for(int i = 0; i < count_regions; i++)
 		      p_store[i] = new_pool[i];
    */

}

bool VMPool::is_legitimate(unsigned long _address) {
 
    // Check if address is in between base_address and base_address + size
   
    int index = 0;     
    if(count_regions ==0){
        //debug_out_E9("IS_LEGITIMATE: TRUE\n");
        return false;
    }

    if((_address < (base_address + size)) && (_address >= base_address)) {

        while(index < count_regions){
          
          if((p_store[index].addr <= _address < (p_store[index].addr+p_store[index].size))){
            //debug_out_E9("IS_LEGITIMATE: TRUE\n");			
            return true;
          }
          index++;
	}
		
    }
    //debug_out_E9("IS_LEGITIMATE: FALSE\n");
    return false;
}

