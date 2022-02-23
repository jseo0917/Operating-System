/*
 * File: manager.H
 *
 * Management of allocation/deallocation of CONTIGUOUS frames.
 * The manager is responsible for n_frames starting on framed first_frame,
 * i.e., it will manage frames first_frame to first_frame + n_frames - 1
 *
 */

#ifndef _MANAGER_H_                   // include file only once
#define _MANAGER_H_


class Manager {

private:
    // You can add whatever you need here
    char* ptr;
    int end_of_memory;
    int offset;

public:
    // public so that tests can check using gtest.C.
    // Students can use whatever values they want
    const char FREE = 3, ALLOCATED = 1, HEAD_OF_SEQUENCE = 2, INACCESSIBLE = 0, ONEBYTE = 8;

    /* The constructor
    **
    ** map_ptr points to the area holding your bitmap/bytemap.
    **        You can assume the pointer you are receiving in the
    **        constructor points to an area with enough space
    **        for managing all frames in this pool
    ** n_frames is the number of frames being managed
    ** base_frame is the first frame in this pool;
    **        you can assume base_frame > 0
    */
    Manager(unsigned long long _map_ptr, // long long to test on
                                         // environments with 64 bit
                                         // architecture
            unsigned long _n_frames,
            unsigned long _base_frame);

    /* get_frames
    **
    ** _n_frames is the number of frames being requested
    **
    ** The method will look for n consecutive frames that are free and return
    ** the id of the first one in the sequence.
    ** If there is no sequence of _n_frames available, the method will return 0
    **/
    unsigned long get_frames(unsigned long _n_frames);


    /* release_frames
    **
    ** Releases a previously allocated contiguous sequence of frames
    ** back to its frame pool.
    ** The frame sequence is identified by the number of the first frame.
    **
    ** It returns true on success and false on failure (i.e., invalid arguments)
    **/
    bool release_frames(unsigned long _first_frame_no);

    /* mark_innacessible
    **
    ** Marks a contiguous area of physical memory, i.e., a contiguous
    ** sequence of frames, as inaccessible.
    ** _starting_frame: Number of first frame to mark as inaccessible.
    ** _n_frames: Number of contiguous frames to mark as inaccessible.
    **
    ** Implementation can assume that arguments are valid
    **/
    void mark_inaccessible(unsigned long _starting_frame,
                           unsigned long _n_frames);

    /* NumberBitsRepresentingFrame
    ** Returns the number of bits your implementation is using to represent
    ** each frame in your 'bitmap' (2 or 8)
    */
    static int NumberBitsRepresentingFrame();

    /* Used by provided unit testing (which you are not required to use)
    **
    ** Notice that argument is the frame id, not the index in the 'bitmap'.
    ** It will return FREE, ALLOCATED, HEAD_OF_SEQUENCE, or INACCESSIBLE
    **
    ** You can assume _frame_nb is a valid argument, i.e, the number of a
    ** frame that this pool is managing
    */
    char get_frame_state(unsigned long _frame_nb);
};
#endif
