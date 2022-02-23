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
ContFramePool* ContFramePool::frame_ptr[5] ={NULL};

int ContFramePool::index_ = 0;
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
	// KERNEL POOL
	if(_info_frame_no == 0){
		
		// GET THE ADDRESS OF FRAME 0		
		ptr = (char*)(_base_frame_no * FRAME_SIZE); // 2048

		// WE ARE USING first FRAME 0, 3 = ALLOCATED
		ptr[0] = 1;
		
		// Make Rest of the frames free	
		for(int i = 1; i < _nframes; i++)
			ptr[i] = 3;
		
	}
	// PROCESS POOL MAKE ALL FRAMES FREE
	else{
		ptr = (char*)(_info_frame_no * FRAME_SIZE);

		for (int i =0; i < _nframes; i++)
			ptr[i] = 3;		
	}
	
	// THESE PARTS ARE FOR RELEASE FUNCTION (STATIC)
	// Keeps all the objects that were created
	if (frame_ptr[0] == NULL){
		index_ = 0;
		frame_ptr[0] = this;
		index_++;
	}
	else{		
		frame_ptr[index_] = this;
		index_++;
	}

	offset = _base_frame_no;
	n_frames = _nframes;
	info_frame_no = _info_frame_no;
	n_info_frames = _n_info_frames;
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
	int index = 0;
	bool found = false;
	int count = 0;
	debug_out_E9_msg_value("Number of FRAME Requested: ", _n_frames);
	
	// WHEN IT REQUIRES TOO MUCH FRAMES
	// RETURN FALSE
	if (_n_frames > n_frames)
			return 0;
	
	// FIND IF WE HAVE CONTIGUOUS FRAMES UP TO N
	while(index < n_frames && count <= _n_frames){
		debug_out_E9_msg_value("GET FRAME", count);
		debug_out_E9_msg_value("PTR[INDEX]", ptr[index]);
		// WHEN PTR AT INDEX IS NOT AVAILABLE
		if((unsigned long)ptr[index] == 2 || (unsigned long)ptr[index] == 0 || (unsigned long)ptr[index] == 1){
		    index++;
		    count = 0;
		}

		// WHEN PTR IS FREE
		else{
		    count ++;
		    //Console::puts("MEMORY TEST FAILED. ERROR IN FRAME POOL\n");Console::puti(count);
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

		ptr[head_of_seq] = 2;
		//debug_out_E9_msg_value("Count", count);
		
		if (count == 1){
			debug_out_E9_msg_value("GET FRAME Success", 1);
			return (unsigned long)(head_of_seq + offset);
		}

		for (int i = head_of_seq + 1; i <= index; i++)
			  ptr[i] = 1;
		debug_out_E9_msg_value("GET FRAME Success", 1);
		return (unsigned long)(head_of_seq + offset);

	}
	// When we do not have enough frames	
	else{
		return 0;
		debug_out_E9_msg_value("GET FRAME FAILED", -1);
	}
	return 0;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // THIS PART IS SAME AS PART A
    int i = 0;

    _base_frame_no -= offset;

    while(i < _n_frames){
      ptr[_base_frame_no] = 0;
      _base_frame_no++;
      i++;
    }

    return;
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
	int i = 0;
	debug_out_E9_msg_value("Release frame ",0);
	// UNTIL I < 5 BECAUSE WE FIRST INITIALIZED THE ARRAY WITH 5 
	while(i <= 5){
		//frame_ptr[i]->offset + frame_ptr[i]->n_frames >= _first_frame_no){
		//debug_out_E9_msg_value("base frame : ",frame_ptr[i]->offset);
		//debug_out_E9_msg_value("n_frames   : ",frame_ptr[i]->n_frames + frame_ptr[i]->offset);
		//debug_out_E9_msg_value("First_frame: ",_first_frame_no);
		//debug_out_E9_msg_value("i          : ", i );
		if((frame_ptr[i]->offset <= _first_frame_no)){
			
			if(_first_frame_no < (frame_ptr[i]->n_frames + frame_ptr[i]->offset)){
						break;
			}
		}		
		i++;

		if(i > 5)
			return;
	}
	
	ContFramePool::release_frames_part_A(i, _first_frame_no);
	
	//ContFramePool::release_frames_part_A(i, _first_frame_no);
	// PRINT OUT TO SEE IF THE RESULT IS TRUE. TRUE = RELEASED CORRECTLY OTHERWISE 0.
	//debug_out_E9_msg_value("Result ",ContFramePool::release_frames_part_A(i, _first_frame_no));
	
	return;
}

bool ContFramePool::release_frames_part_A(int i, unsigned long _first_frame_no)
{		 
    // SAME AS PART A
    ContFramePool* area = frame_ptr[i];
	
    _first_frame_no = _first_frame_no - frame_ptr[i]->offset;

    if((unsigned long)area->ptr[_first_frame_no] != 2){
        return false;
    }
	
    area->ptr[_first_frame_no] = 3;

    _first_frame_no++;
   
    while((unsigned long)area->ptr[_first_frame_no] != 3 && (unsigned long)area->ptr[_first_frame_no] != 2 && (unsigned long)area->ptr[_first_frame_no] != 1){

        // cout << "FRAME NUM: " << _first_frame_no << " STATE: " << (unsigned long)ptr[_first_frame_no] << endl;
        if(_first_frame_no >= area->n_frames)
                              break;

        area->ptr[_first_frame_no] = 3;

        _first_frame_no++;

    }
	
    return true;
	

}
unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    
	return _n_frames / (4096) + (_n_frames % (4096) > 0 ? 1 : 0);
}
