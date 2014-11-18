#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#include "syscall.h"
#include "thread.h"

#define MAX_PROCESS 128

class Table 
{
public:
   /* Create a table to hold at most "size" entries. */
   Table(int size) 

   ~Table();

   /* Allocate a table slot for "object", returning the "index" of the
      allocated entry; otherwise, return -1 if no free slots are available. */
   int Alloc(void *object)

   /* Retrieve the object from table slot at "index", or NULL if that
      slot has not been allocated. */
   void *Get(int index) 

   /* Free the table slot at index. */
   void Release(int index)

private:
   Semaphore *mutex;
   int *list;
   int *value;
}

#endif // PROCESSTABLE_H
