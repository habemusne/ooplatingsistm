#include "table.h"

/* Create a table to hold at most "size" entries. */
Table::Table(int size)
{
   mutex = new Semaphore("Mutex", 1);
   list = new int[MAX_PROCESS];
   value = new int[MAX_PROCESS];
   for(int i = 0; i < MAX_PROCESS; i++)
      value[i] = 0;
}

Table::~Table()
{
   for(int i = 0; i < MAX_PROCESS; i++){
      Release(i);
   }

   delete mutex;
   delete value;
   delete list;
}


/* Allocate a table slot for "object", returning the "index" of the
   allocated entry; otherwise, return -1 if no free slots are available. */
int Table::Alloc(void *object)
{
   mutex->P();
   int i;
   for(i = 0; i < MAX_PROCESS; i++)
      if(value[i] == 0)
      {
         value[i] = 1;
         object = (void *)(list + i);
         break;
      }
   mutex->V();

   if (i < MAX_PROCESS)
      return i;
   else 
      return -1;
}

/* Retrieve the object from table slot at "index", or NULL if that
   slot has not been allocated. */
void* Table::Get(int index) 
{
   if(index >= 0 && index < MAX_PROCESS)
      return (void *)(list + index);
   else
      return NULL;
}

/* Free the table slot at index. */
void Table::Release(int index)
{
   mutex->P();
   value[index] = 0;
   if(index >= 0 && index < MAX_PROCESS)
      delete (list + index);

   mutex->V();
}
