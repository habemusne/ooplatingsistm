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
   mutex->P();
   for(int i = 0; i < MAX_PROCESS; i++){
      Release(i);
   }

   delete mutex;
   delete value;
   delete list;
   mutex->V();
}


/* Allocate a table slot for "object", returning the "index" of the
   allocated entry; otherwise, return -1 if no free slots are available. */
int Table::Alloc(void *object)
{
   mutex->P();
   int i;
   for(i = 2; i < MAX_PROCESS; i++)
      if(value[i] == 0)
      {
         value[i] = 1;
         list[i] = (int)object;
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
      return (void*)list[index];
   else
      return NULL;
}

/* Free the table slot at index. */
void Table::Release(int index)
{
   mutex->P();
   value[index] = 0;
   if(index >= 0 && index < MAX_PROCESS)
      list[index] = 0;

   mutex->V();
}

int Table::GetIndex(void *thread)
{
   for(int i = 1; i < MAX_PROCESS; i++)
   {
      if(table->Get(i) == thread)
         return i;
   }

   return -1;
}

int Table::Alloc_mainThread(void *object)
{
   //index 1 is reserved for main thread
   mutex->P();
   value[1] = 1;
   list[1] = (int)object;
   mutex->V();

   return 1;
}
