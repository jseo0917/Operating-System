/*
** File: gtest.C
*/

#include <iostream>
using std::cout, std::endl;
#include <gtest/gtest.h>
#include "manager.H"

/*
** Check the documentation on your environment about how to compile
** with the gtest library.
** On a Mac, I do:
**
** g++ --std=c++17 -Wall manager.C gtest.C -lgtest -lgtest_main -pthread
**/

int main(int argc, char** argv) {
    // initialize gtest environment
    ::testing::InitGoogleTest(&argc, argv);

    // Testing with a pool for frames 1024 to 1535
    const unsigned long POOL_NB_FRAMES = 512;
    const unsigned long BASE_FRAME = 1024;
    unsigned int bytes_needed =
               POOL_NB_FRAMES / (8 / Manager::NumberBitsRepresentingFrame());
    char* area = new char[bytes_needed];
    Manager frame_pool = Manager((unsigned long long) area,
                                 POOL_NB_FRAMES, BASE_FRAME);

    // create a hole starting at frames 1048 and ending at frame 1051
    frame_pool.mark_inaccessible(1048, 4);
    // testing mark_innacessible
    EXPECT_EQ (frame_pool.INACCESSIBLE, frame_pool.get_frame_state(1048));
    EXPECT_EQ (frame_pool.INACCESSIBLE, frame_pool.get_frame_state(1051));
    EXPECT_NE (frame_pool.INACCESSIBLE, frame_pool.get_frame_state(1052));
    cout << "Tests for mark_inaccessible passed" << endl;

    /* testing get_frames
     */

    // only one frame
    unsigned long frame1 = frame_pool.get_frames(1);
    EXPECT_GE(frame1, BASE_FRAME);
    EXPECT_LT(frame1, BASE_FRAME + POOL_NB_FRAMES);
    EXPECT_EQ(frame_pool.HEAD_OF_SEQUENCE, frame_pool.get_frame_state(frame1));


    unsigned long frame4 = frame_pool.get_frames(4);
    EXPECT_GE(frame4, BASE_FRAME);
    EXPECT_LT(frame4, BASE_FRAME + POOL_NB_FRAMES);
    EXPECT_EQ(frame_pool.HEAD_OF_SEQUENCE, frame_pool.get_frame_state(frame4));
    EXPECT_EQ(frame_pool.ALLOCATED, frame_pool.get_frame_state(frame4+1));
    EXPECT_EQ(frame_pool.FREE, frame_pool.get_frame_state(frame4+4));

    // asking for 100 frames; it has to be after the innacessible 'hole'
    unsigned long frame100 = frame_pool.get_frames(100);
    EXPECT_GE(frame100, 1052);
    EXPECT_LT(frame100, BASE_FRAME + POOL_NB_FRAMES);
    EXPECT_EQ(frame_pool.HEAD_OF_SEQUENCE, frame_pool.get_frame_state(frame100));
    EXPECT_EQ(frame_pool.ALLOCATED, frame_pool.get_frame_state(frame100+99));
    EXPECT_EQ(frame_pool.FREE, frame_pool.get_frame_state(frame100+100));

    // asking for too many frames
    unsigned long frame2048 = frame_pool.get_frames(2048);
    EXPECT_EQ(frame2048, 0);

    cout << "Tests for get_frames passed" << endl;

    /* testing release_frames
     */

    // try to release a frame that is allocated but not head of sequence
    EXPECT_EQ(false, frame_pool.release_frames(frame100+1));

    // try to release a frame in the memory 'hole' (inaccessible)
    EXPECT_EQ(false, frame_pool.release_frames(1048));

    // try to release frame that should be free
    EXPECT_EQ(false, frame_pool.release_frames(frame100+100));

    // release allocation that returned frame1
    EXPECT_EQ(true, frame_pool.release_frames(frame1));
    EXPECT_EQ(frame_pool.FREE, frame_pool.get_frame_state(frame1));

    // cout << "\nRelease frame 4 " << frame4 + 3 - 1024<< endl;
    // release allocation that returned frame4
    EXPECT_EQ(true, frame_pool.release_frames(frame4));
    EXPECT_EQ(frame_pool.FREE, frame_pool.get_frame_state(frame4));
    EXPECT_EQ(frame_pool.FREE, frame_pool.get_frame_state(frame4+3));

    cout << "Tests for release_frames passed" << endl;

    // Add here other test cases you find helpful
}
