#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "synch.h"
#include "bitmap.h"

class MemoryManager
{
   public:
      MemoryManager(int numPages);
      ~MemoryManager();
      int AllocPage();
      void FreePage(int physPageNum);
      bool PageIsAllocated(int physPageNum);

   private:
      BitMap *physPages;
      Semaphore *mutex;
};

extern MemoryManager *memoryManager;
#endif
