/*
    File: kernel.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University

    Last modified by Dilma Da Silva on Fall 2021.

    The "main()" function is the entry point for the kernel.

*/



/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "console.H"
#include "utils.H"

using namespace std;

/* ======================================================================= */
/* MAIN -- THIS IS WHERE THE OS KERNEL WILL BE STARTED UP */
/* ======================================================================= */

int main()
{

  /* -- INITIALIZE CONSOLE */
  Console::init();
  Console::puts("Initialized console.\n");
  Console::puts("\n");

  Console::puts("Example of selected output in the screen\n");
  Console::puts("with details redirected to a file for debugging purposes:\n");
  for (unsigned int i = 0; i < 1000; i++) {
    // information go to the bochs output to be captured in a file
    debug_out_E9_msg_value("i is ", i);

    // selected output on the console screen, since it goes too fast!!
    if (i % 100 == 0) {
        Console::puts("i is ");
        Console::putui(i);
        Console::puts("\n");
    }
  }

  Console::puts("\n\nReplace the following <Name> field with your name.\n");
  Console::puts("After your are done admiring your output, you can shutdown this 'machine'.\n");
  Console::puts("\n");
  Console::puts("WELCOME TO MY KERNEL!\n");
  Console::puts("      ");
  Console::set_TextColor(GREEN, RED);
  Console::puts("<Jung Hoon Seo>\n");

  /* -- LOOP FOREVER! */
  for(;;);

}
