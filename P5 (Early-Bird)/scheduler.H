/* 
    Author: R. Bettati, Joshua Capehart
            Department of Computer Science
            Texas A&M University
			
	    A thread scheduler.

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "thread.H"
#include "console.H"
#include "utils.H"

/*--------------------------------------------------------------------------*/
/* !!! IMPLEMENTATION HINT !!! */
/*--------------------------------------------------------------------------*/
/*
    One way to proceed is to implement the FIFO scheduling policy inside
    class 'Scheduler'. 

    If you plan to implement a Round-Robin Scheduler, derive it from class
    'Scheduler', say as class 'RRScheduler'. The class 'RRScheduler' is 
    really just a FIFO scheduler with THREE MODIFICATIONS:
    1. It manages a timer, which fires at the end-of-quantum (EOQ). 
    (For details on how to set up a timer and how to handle timer interrupts 
    see the 1-second timer in 'kernel.C'.)  The timer is set up in the
    constructor.
    2. It uses an additional function, the EOQ handler. This function gets
    called whenever an EOQ timer event fires. The EOQ handler forces the 
    current thread to call the scheduler's 'yield' function.
    3. The 'yield' function must be modified to account for unused quantum
    time. If a thread voluntarily yields, the EOQ timer must be reset in order
    to not penalize the next thread.
 
    (Note that this qualifies as programming at about the level of a baboon.
     Much better woudl be to have the abstract class 'Scheduler' implement 
     the basic scheduling MECHANISMS and provide abstract funtions to define
     the queue management POLICIES in derived classes, 
     such as 'FIFOScheduler'.)
    
 */

/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/

struct Node{
	Thread* thread;
	Node* next;
	Node (Thread* thread): thread(thread),next(NULL){};
};

class queue{
	
private:
	Node* head; // For dequeue
	Node* tail; // For enqueue
public:
	queue(){
		head = NULL;
		tail = NULL;
	};
	void enqueue(Thread* thread){
		Node* newNode = new Node(thread);
		if((head==NULL) && (tail==NULL)){
			head = newNode;
			tail = newNode;
			return;
		}	 
		tail->next = newNode;
		tail = newNode;
		return;
	};
	
	Thread* dequeue(){
		Node* temp = head;
		if(isEmpty())
			return 0;
		else if (head == tail){
			head = NULL;
			tail = NULL;
		}
		else{
			head = head->next;
		}
		
		Thread* thd = temp->thread;
		delete temp;
		return thd;
	
	};

	Thread* front(){
		return head->thread;
	};

	bool isEmpty(){
		if(head == NULL)
			return true;

		return false;
	};


	
};
class Scheduler {

  /* The scheduler may need private members... */
  // Keeps track of the list of threads in which order they executes
  // Queue (linked list or c++ data structure) 
private:
  queue q;

public:

   Scheduler();
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed in the constructor as well. */

   /* NOTE: We are making all functions virtual. This may come in handy when
            you want to derive RRScheduler from this class. */
  
   virtual void yield();
   /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'Thread.H' to
      do the context switch. */
   // if queue is empty do nothing
   // else remove from queue and call dispatch_to

   virtual void resume(Thread * _thread);
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */
   // Put me in the queue (end of the queue) 
   // When thread already exists

   virtual void add(Thread * _thread);
   /* Make the given thread runnable by the scheduler. This function is called
      after thread creation. Depending on implementation, this function may 
      just add the thread to the ready queue, using 'resume'. */

   // end of the queue (for the new thread)

   virtual void terminate(Thread * _thread);
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. 
      Graciously handle the case where the thread wants to terminate itself.*/
   
   // You schedule next thread
};
	
	

#endif
