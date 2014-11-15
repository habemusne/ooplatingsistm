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
   delete MemManager;
}

int MemoryManager::AllocPage()
{
   mutex->P();
   int result = physPages.find();
   mutex->V();
   return result;
}

void MemoryManager::FreePage(int physPageNum)
{
   mutex->P();
   physPages->Clear(physPageNum);
   mutex->V();
}

bool PageIsAllocated(int physPageNum) {
   return physPages->Test(physPageNum);
}
