#include "memorymanager.h"
#include "machine.h"

MemoryManager* MemoryManager = NULL;

MemoryManager*
MemoryManager::GetInstance()
{
  if(!manager)
    manager = new MemoryManager(NumPhysPages);
  return manager;
}

MemoryManager::MemoryManager(int numPages)
{
  pages = new BitMap(numPages);
  mmLock = new Lock("mmLock");
  usedPages = 0;
  totalPages =  numPages;
  replaceIndex = 0;
  entries = new Record[totalPages];
  for(int i = 0; i < totalPages; i++)
  {
    entries[i].allocated = false;
    entries[i].ioLocked = false;
    entries[i].space = NULL;
    entries[i].vPageNumber = -1;

  }
}

MemoryManager::~MemoryManager()
{
  delete pages;
  delete mmLock;
  delete manager;
  delete[] entries;
}

int
MemoryManager::AllocPage()
{
  if(usedPages >= totalPages)
    return -1;

  mmLock->Acquire();
  int pageNumber = pages->Find(); //locate next free page
  if(pageNumber >= 0)
    usedPages++; //free page discovered
  entries[pageNumber].allocated = true;
  mmLock->Release();
  return pageNumber;
}

void
MemoryManager::FreePage(int physPageNum)
{
  if(usedPages <= 0)
    return false;

  mmLock->Acquire();
  pages->Clear(physPageNum);
  usedPages--;
  entries[physPageNum].vPageNumber = -1;
  entries[physPageNum].ioLocked = false;
  mmLock->Release();

  return true;
}

bool PageIsAllocated(int physPageNum) {
  return pages->Test(physPageNum);
}

