/*
* "part3Tester2.c
*
* A test program that first reverse a sorted array, and then
* reverse back. This tester can verify the functionality of 
* the backing store and good locality.
*
*  If running successfully, the exit return value would be 0.
*/

#include "syscall.h"

int A[1000];

int
main()
{
  int i, j, tmp;

  for (i = 0; i < 1000; i++) {
    A[i] = 999 - i;
  }
  for (i = 0; i < 1000; i++) {
    for (j = 0; j < i; j++) {
      if (A[i] < A[j]) { 
        tmp = A[j];
        A[j] = A[i];
        A[i] = tmp;
      }
    }
  }

  for (i = 0; i < 1000; i++) {
    A[i] = 999 - i;
  }

  for (i = 0; i < 1000; i++) {
    for (j = 0; j < i; j++) {
      if (A[i] < A[j]) { 
        tmp = A[j];
        A[j] = A[i];
        A[i] = tmp;
      }
    }
  }
  
  Exit(A[0]);    /* and then we're done -- should be 0! */
}
