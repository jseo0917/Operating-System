/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _nframes,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    base_frame_no = _base_frame_no;
    nframes = _nframes;
    info_frame_no = _info_frame_no;
      
    int n_info_frames;

    if (info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE);
        n_info_frames = needed_info_frames(nframes);
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
        n_info_frames = _n_info_frames;
    }
    
    // Console::puts("bitmap frame = "); Console::puti((unsigned long)bitmap >> 12); Console::puts("\n");
    // Console::puts("n_info_frames = "); Console::puti(n_info_frames); Console::puts("\n");

    for(int i = 0; i < n_info_frames; i++) {
        bitmap[i] = USED;
    }

    for(int i = n_info_frames; i < _nframes; i++) {
        bitmap[i] = FREE;
    }
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames) {  
  // Console::puts("get_frames(");
  // Console::puti(_n_frames);
  // Console::puts(")\n");

  //print_bitmap();

  for(int f1 = base_frame_no; f1 < base_frame_no + nframes - _n_frames; f1++) {
    int l = 0;
    for(int f2 = 0; f2 < _n_frames; f2++) {
      //      Console::puts("f1 + f2 = "); Console::puti(f1+f2); Console::puts("\n");
      //      SimpleKeyboard::wait();
      if (frame_status(f1+f2) != FREE) {
	break;
      } else {
	//	Console::puts("increasing l...");
	l++;
	//	Console::puts("done\n");
      }
    }
    if (l == _n_frames) {
      //      Console::puts("found l = "); Console::puti(l); Console::puts("\n");
      mark_frame(f1, HOS);
      for(int f2 = 1; f2 < _n_frames; f2++) {
	mark_frame(f1 + f2, USED);
      }
      //      Console::puts("get_frames returns ");
      //      Console::puti(f1);
      //      Console::puts("\n");
      //      SimpleKeyboard::wait();
      return f1;
    }
  }
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
  for(int f = _base_frame_no; f < _base_frame_no + _n_frames; f++) {
    mark_frame(f, USED);
  }
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
  
  ContFramePool * pool = list;
  while (pool != NULL && !pool->in_range(_first_frame_no)) {
    pool = pool->next;
  }

  // Check if frame is in any pool!
  assert(pool);

  pool->fp_release_frames(_first_frame_no);
  
}

void ContFramePool::fp_release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
  assert(in_range(_first_frame_no) && frame_status(_first_frame_no) == HOS);

  mark_frame(_first_frame_no, FREE);

  unsigned int f = _first_frame_no + 1;

  while ( in_range(f)  && 
	  (frame_status(f) != FREE) &&
	  (frame_status(f) != HOS)) {
	   mark_frame(f, FREE);   
	 }
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
  return _n_frames / FRAME_SIZE + (_n_frames % FRAME_SIZE > 0 ? 1 : 0);
  // We round up.
}

void ContFramePool::mark_frame(unsigned long _frame_no, unsigned char _status) {
  //  Console::puts("marking frame "); 
  //  Console::puti(_frame_no);
  //  Console::puts(" to ");
  //  print_status(_status);
  //  Console::puts("\n");
  bitmap[_frame_no - base_frame_no] = _status;
}

unsigned char ContFramePool::frame_status(unsigned long _frame_no) {
  //  Console::puts("Checking frame status "); Console::puti(_frame_no);
  //  Console::puts(": ");
  unsigned char status = bitmap[_frame_no - base_frame_no];
  //  print_status(status);
  //  Console::puts("\n");
  return status;
}

bool ContFramePool::in_range(unsigned long _frame_no) {
  return _frame_no >= base_frame_no && _frame_no < base_frame_no + nframes; 
}

ContFramePool * ContFramePool::list = NULL;

void ContFramePool::print_status(unsigned char _status) {
  switch(_status) {
  case FREE:
    Console::puts("F");
    break;
  case HOS:
    Console::puts("H");
    break;
  case USED: 
    Console::puts("U");
    break;
  default:
    Console::puts("<<<UNKOWN>>>");
  }
}

void ContFramePool::print_bitmap() {
  /* Prints the beginning of the bitmap. */
  Console::puts("bitmap: ");
  for(int i = 0; i < 20; i++) {
    print_status(bitmap[i]);
  }
  Console::puts("\n");
}
