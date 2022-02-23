/*
** File: manager.C

Your implementation needs to maintain free *sequences* of frames.
This can be done in many ways, ranging from bitmaps to free-lists of frames etc.

IMPLEMENTATION:

 One simple way to manage sequences of free frames is to add a minor
 extension to the idea of bitmaps: instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame,
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD_OF_SEQUENCE.
 If a frame is marked as HEAD_OF_SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.

 If we were keeping track only of FREE and ALLOCATED, we would need only
 one bit to represent the state of a frame in the pool.
 But with three possible states (FREE, ALLOCATED, HEAD_OF_SEQUENCE), we need
 at least two bits to present the state of a frame.

 Using two bits to present the state of a frame means that each element of the
 array (of chars) holding the bitmap will be representing four frames.
 If you want to simplify your programming and have one frame per array element
 (a bytemap instead of bitmap) you are wasting memory.

 You can choose between adoption a bitmap or bytemap (there is a bonus for using
 a bitmap)

 DETAILED IMPLEMENTATION:

 How can we use the HEAD_OF_SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:

 Constructor: Initialize all frames to FREE

 get_frames(n_frames): Traverse the "bitmap" of states and look for a
 sequence of at least _n_frames entries that are FREE. If you find one,
 mark the first one as HEAD_OF_SEQUENCE and the remaining n_frames-1 as
 ALLOCATED.

 release_frames(first_frame_no): Check whether the first frame is marked as
 HEAD_OF_SEQUENCE. (If not, something went wrong.) If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or
 HEAD_OF_SEQUENCE. Until then, mark the frames that you traverse as FREE.

 Notice that the API also requires to keep track of innacessible frames,
 so you have a frame can be in one of four states:
 FREE, ALLOCATED, HEAD_OF_SEQUENCE, and INACCESSIBLE

--------------------------------------------------------------------------*/

#include "manager.H"
#include <assert.h>
#include <iostream>
// Changed first argument from unsigned long to unsigned long long
// to compile in some VSCode environments
using namespace std;
Manager::Manager(unsigned long long _map_ptr,
                unsigned long _n_frames,
                unsigned long _base_frame)
{
    // TODO: IMPLEMENTATION NEEDED!

    // Initialize all memory to Free
    ptr = (char*)_map_ptr;

    for(int i = 0; i < _n_frames; i++){
          ptr[i] = FREE;
    }

    // Init end of memory;
    end_of_memory = _n_frames;

    offset = _base_frame;
    //assert(false);
}

unsigned long Manager::get_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEDED!
    int index = 0;
    bool found = false;
    int count = 0;

    // cout << "\nGET " << _n_frames << " FRAMES\n";

    if(_n_frames >end_of_memory )
                      return false;
    // When ptr has reached at the end of memory, and no free memory is available
    while(index < end_of_memory && count <= _n_frames){

        // WHEN PTR AT INDEX IS NOT AVAILABLE
        if((unsigned long)ptr[index] == HEAD_OF_SEQUENCE || (unsigned long)ptr[index] == INACCESSIBLE || (unsigned long)ptr[index] == ALLOCATED){
            index++;
            count = 0;
        }

        // WHEN PTR IS FREE
        else{
            count ++;

            if(count >= _n_frames){
                found = true;
                break;
            }// Found n frames, end while

            index++;
        }

    }// END WHILE

    // WHEN THE FRAME IS FOUAD, SET THE FIRST FRAME AS HEAD_OF_SEQUENCE. REMAINING TO ALLOCATED UNTIL _N_FRAMES
    if(found){

        int head_of_seq = index - (count - 1);

        // cout << "FROM " << head_of_seq << " TO " << index << endl << endl;;

        ptr[head_of_seq] = HEAD_OF_SEQUENCE;

        if (count == 1){
            return (unsigned long)(head_of_seq + offset);
        }

       for (int i = head_of_seq + 1; i <= index; i++)
                  ptr[i] = ALLOCATED;

       return (unsigned long)(head_of_seq + offset);

    }
    else
        return false;

    return false;

    //assert(false);
}

bool Manager::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEDED!

    // cout << "FRAME NUM: " << _first_frame_no << endl;

    _first_frame_no = _first_frame_no - offset;

    // cout << "FRAME NUM STATE " << (unsigned long)ptr[_first_frame_no] << endl;

    if((unsigned long)ptr[_first_frame_no] != HEAD_OF_SEQUENCE){

        //std::cerr << "The starting frame is not HEAD_OF_SEQUENCE" << std::endl;

        return false;
    }

    ptr[_first_frame_no] = FREE;

    // cout << "FRAME NUM: " << _first_frame_no << " STATE: " << (unsigned long)ptr[_first_frame_no] << endl;

    _first_frame_no++;
    // cout << "FRAME NUM: " << _first_frame_no << " STATE: " << (unsigned long)ptr[_first_frame_no] << endl;
    while((unsigned long)ptr[_first_frame_no] != FREE && (unsigned long)ptr[_first_frame_no] != HEAD_OF_SEQUENCE && (unsigned long)ptr[_first_frame_no] != INACCESSIBLE){

        // cout << "FRAME NUM: " << _first_frame_no << " STATE: " << (unsigned long)ptr[_first_frame_no] << endl;
        if(_first_frame_no >= end_of_memory)
                              break;

        ptr[_first_frame_no] = FREE;

        //cout << "FRAME NUM: " << _first_frame_no << " STATE: " << ptr[_first_frame_no] << endl;
        _first_frame_no++;

    }
    // cout << "FRAME NUM: " << _first_frame_no << " STATE: " << (unsigned long)ptr[_first_frame_no] << endl;
    return true;
    //assert(false);
}

void Manager::mark_inaccessible(unsigned long _starting_frame,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEDED!
    int i = 0;

    _starting_frame -= offset;

    // cout << "\nFROM " << _starting_frame << " TO ";

    while(i < _n_frames){
      ptr[_starting_frame] = INACCESSIBLE;
      _starting_frame++;
      i++;
    }

    // cout << _starting_frame << " INACCESSIBLE\n\n";

    return;

    //assert(false);
}
int Manager::NumberBitsRepresentingFrame() {
    // TODO: IMPLEMENTATION NEEDED!
    return 8;
}

char Manager::get_frame_state(unsigned long _frame_nb) {
    // TODO: if you want to use it in testing, you need to implement it

    _frame_nb -= offset;

    return ptr[_frame_nb];
    //assert(false);
    //return 255; // returning garbage state to compile cleanly
}
