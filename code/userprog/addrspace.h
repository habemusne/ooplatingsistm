// addrspace.h
//	Data structures to keep track of executing user programs
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "memorymanager.h"
#include "noff.h"

#define UserStackSize		1024 	// increase this as necessary!
class MemoryManager;
extern MemoryManager* memoryManager;


class AddrSpace {
public:
    AddrSpace(bool isProg, int spaceId);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    char* vir_to_phys(unsigned int virtual_addr);

    void InitRegisters();		// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

    /*NAN CHEN*/
    //int Initialize(OpenFile *executable, bool isProgTesti, char** arg_vird, int argumentSize);
    int Initialize(OpenFile *executable);
    /*NAN CHEN*/
    int readFromExecutable(int faultAddr);

    unsigned int getNumPages();

    bool* execOrBS;//0 indicates need to be load from exec, 1 from bs
    int* phys_page_i_to_virt_page;//which virt_page is possessing ith phys_page
    int spaceId;

private:
    bool isProgtest;
    TranslationEntry *pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;		// Number of pages in the virtual
    OpenFile *executable;
    NoffHeader noffH;
    // address space
    //int argument_size;
    //char** argument_addr;
};

#endif // ADDRSPACE_H
