/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "machine.H"
#include "thread.H"
#include "scheduler.H"
extern Scheduler* SYSTEM_SCHEDULER;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
   disk_id   = _disk_id;
   disk_size = _size;
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::issue_operation(DISK_OPERATION _op, unsigned long _block_no) {

  Machine::outportb(0x1F1, 0x00); /* send NULL to port 0x1F1         */
  Machine::outportb(0x1F2, 0x01); /* send sector count to port 0X1F2 */
  Machine::outportb(0x1F3, (unsigned char)_block_no);
                         /* send low 8 bits of block number */
  Machine::outportb(0x1F4, (unsigned char)(_block_no >> 8));
                         /* send next 8 bits of block number */
  Machine::outportb(0x1F5, (unsigned char)(_block_no >> 16));
                         /* send next 8 bits of block number */
  Machine::outportb(0x1F6, ((unsigned char)(_block_no >> 24)&0x0F) | 0xE0 | (disk_id << 4));
                         /* send drive indicator, some bits, 
                            highest 4 bits of block no */

  Machine::outportb(0x1F7, (_op == READ) ? 0x20 : 0x30);

}



bool BlockingDisk::is_ready() {
   return ((Machine::inportb(0x1F7) & 0x08) != 0);
}

void BlockingDisk::wait_until_ready(){
		while(!is_ready()){ 
			Console::puts("is not ready\n");
			SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
			SYSTEM_SCHEDULER->yield();
		} 
}

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {

  Console::puts("You must implement BlockingDisk::read\n");
  // -- REPLACE THIS with code that won't do busy waiting
  //SimpleDisk::read(_block_no, _buf);

    issue_operation(READ, _block_no);

    wait_until_ready();
  
	  /* read data from port */
    int i;
    unsigned short tmpw;
    for (i = 0; i < 256; i++) {
	    tmpw = Machine::inportw(0x1F0);
	    _buf[i*2]   = (unsigned char)tmpw;
	    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
    }
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  Console::puts("You must implement BlockingDisk::write\n");
  // -- REPLACE THIS with code that won't do busy waiting
  //SimpleDisk::write(_block_no, _buf);

  issue_operation(WRITE, _block_no);

  wait_until_ready();

  /* write data to port */
  int i; 
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }

}
