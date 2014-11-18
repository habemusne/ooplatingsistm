#ifndef MEMMANAGER_H
#define MEMMANAGER_H

#include "synch.h"
#include "bitmap.h"
#include "addrspace.h"

class Record
{
 public:
    bool allocated;
    AddrSpace* space;
    int vPageNumber;
    bool ioLocked;
};

class MemoryManager
{
 public:
    MemoryManager(int numPages);
    ~MemoryManager();
    int AllocPage();
    void FreePage(int physPageNum);
    bool PageIsAllocated(int physPageNum);
    Record* entries;
    int replaceIndex;
    int totalPages;
 private:
    BitMap* pages;
    int usedPages;
    Lock* mmLock;
};

#endif