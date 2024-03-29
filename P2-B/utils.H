/*
     File        : utils.H

     Author      : Riccardo Bettati
     Modified    : 09/02/12

     Description : Various definitions (NULL/BOOL/TRUE/FALSE) and
                   utility functions (e.g. abort, memory and 
                   string functions).
*/

#ifndef _utils_h_
#define _utils_h_

/*---------------------------------------------------------------*/
/* GENERAL CONSTANTS */
/*---------------------------------------------------------------*/

#ifndef NULL
#   define NULL 0
#endif

#ifdef BOOLEAN
# undef BOOLEAN
#endif
#define BOOLEAN int

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif



/*---------------------------------------------------------------*/
/* ABORT */
/*---------------------------------------------------------------*/

void abort();
/* Stop execution. */

/*********************************************************
 * Debugging
 *********************************************************/

void debug_out_E9(const char* _string);
void debug_out_E9_msg_value(const char* msg, unsigned int value);


/*---------------------------------------------------------------*/
/* SIMPLE MEMORY OPERATIONS */
/*---------------------------------------------------------------*/

void *memcpy(void* dest, const void* src, int count);
/* Copy _count bytes from _src to _dest. (No check for uverlapping) */

void *memset(void* dest, char val, int count);
/* Set _count bytes to value _val, starting from location _dest. */

unsigned short *memsetw(unsigned short* dest, unsigned short val, int count);
/* Same as above, but operations are 16-bit wide. */

/*---------------------------------------------------------------*/
/* SIMPLE STRING OPERATIONS (STRINGS ARE NULL-TERMINATED) */
/*---------------------------------------------------------------*/

int strlen(const char* _str);
/* Determine the length of null-terminated string. */

void strcpy(char* _dst, char* _src);
/* Copy null-terminated string from _src to _dst. */

void strncat ( char *dest, char *src, int num );

void int2str(int _num, char* _str);
/* Convert int to null-terminated string. */

void uint2str(unsigned int _num, char* _str);
/* Convert unsigned int to null-terminated string. */

void ulong2hexstr(unsigned long _num, char* _str);
    
/*---------------------------------------------------------------*/
/* PORT I/O OPERATIONS */
/*---------------------------------------------------------------*/

char inportb  (unsigned short _port);
unsigned short inportw (unsigned short _port);
/* Read data from input port _port.*/

void outportb (unsigned short _port, char _data);
void outportw (unsigned short _port, unsigned short _data);
/* Write _data to output port _port.*/


#endif



