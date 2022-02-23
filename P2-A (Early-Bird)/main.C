/*
** File: main.C
*/

#include "manager.H"
#include <gtest/gtest.h>
#include <iostream>
using namespace std;
int main() {
    // Testing with a pool for frames 1024 to 1535
    const unsigned long POOL_NB_FRAMES = 512;
    const unsigned long BASE_FRAME = 1024;

    unsigned int bytes_needed =
               POOL_NB_FRAMES / (8 / Manager::NumberBitsRepresentingFrame());
    //cout << bytes_needed << endl;

    char* area = new char[bytes_needed];

    // unsigned long long so that it can run cleanly in environments
    // where unsigned long is 4 byte-long and char* is 8 byte-long
    Manager frame_pool = Manager((unsigned long long) area,
                                 POOL_NB_FRAMES, BASE_FRAME);

    // create a hole starting at frames 1048 and ending at frame 1051
    frame_pool.mark_inaccessible(1048, 4);

    //EXPECT_EQ (frame_pool.INACCESSIBLE, frame_pool.get_frame_state(1048));
    // Test your implementation by invoking get_frames and release_frames
    // If you have the Google Test framework on your environment, you can
    // use the tests in gtest.C
    // (You can easily find guides to install the Google Test Framework on
    // Mac, Windows, and Linux environments)

}
