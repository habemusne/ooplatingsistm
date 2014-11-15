#include "memorymanager.h"



MemoryManager::MemoryManager(int numPages)
{
   physPages = new BitMap(numPages);
   mutex = new Semaphore("Mutex", 1);
}

MemoryManager::~MemoryManager()
{
   delete physPages;
   delete mutex;
   //delete memoryManager;
}

int MemoryManager::AllocPage()
{
   mutex->P();
   int result = physPages->Find();
   mutex->V();
   return result;
}

void MemoryManager::FreePage(int physPageNum)
{
   mutex->P();
   physPages->Clear(physPageNum);
   mutex->V();
}

bool MemoryManager::PageIsAllocated(int physPageNum) {
   mutex->P();
   int result = physPages->Test(physPageNum);
   mutex->V();
   return result;
}
