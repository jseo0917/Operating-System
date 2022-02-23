/*
 File: scheduler.C
 
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

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

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
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/


Scheduler::Scheduler() {
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  // If there is some threads in a queue, remove it and call dispatch_to
  // Else queue is empty so  do nothing
  Machine::disable_interrupts();
  if (!q.isEmpty()){
    Machine::enable_interrupts();
	  Thread::dispatch_to(q.dequeue());    
  }
  
  return;
}

void Scheduler::resume(Thread * _thread) {
  // Put the thread at the end of the queue
  Machine::disable_interrupts();
  q.enqueue(_thread);
  Machine::enable_interrupts();
  return;
}

void Scheduler::add(Thread * _thread) {
  // Put the thread at the end of the queue
  Machine::disable_interrupts();
  q.enqueue(_thread);
  Machine::enable_interrupts();
  return;
}

void Scheduler::terminate(Thread * _thread) { 
  
  queue temp;
  
  Machine::disable_interrupts();

  //Find the current thread from the queue and then remove it.
  while(!q.isEmpty()){
	Thread* thd = q.dequeue();

    if (thd->ThreadId() != _thread->ThreadId()){
      temp.enqueue(thd);
      debug_out_E9("Enqueued\n");
    }
  } //END WHILE
  q = temp;
  Machine::enable_interrupts();
  // Dispatch the next thread
  yield();
    
}
