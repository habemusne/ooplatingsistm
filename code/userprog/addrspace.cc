// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
  /*NAN CHEN*/
  //
  //All previous code is moved to Initialize() method. In Exec(), Exec()
  //  needs to firstly call this constructor, and then call 
  //  addrSpace->Initialize();
  //
  this->numPages = 0;
  this->pageTable = NULL;
  /*NAN CHEN*/
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
  /*NAN CHEN*/
  for (unsigned int i = 0; i < numPages; ++i) {
    memoryManager->FreePage(pageTable[i].physicalPage);
  }
  /*NAN CHEN*/

  delete [] pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

char* AddrSpace::vir_to_phys(unsigned int virtual_addr)
{
   return &machine->mainMemory[pageTable[virtual_addr/PageSize].physicalPage * PageSize + virtual_addr % PageSize];
}



int AddrSpace::Initialize(OpenFile *executable) {
    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
            (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    printf("[machine.h] Total memory size: %d\n", MemorySize);
    printf("[machine.h] PageSize: %d\n", PageSize);


    // how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
           + UserStackSize; // we need to increase the size

    printf("Acutal address Space Size: %d\n", size);
    // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    printf("numPages = %d, size allocated = numPages * PageSize = %d\n", numPages, size);

    ASSERT(numPages <= NumPhysPages);   // check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);
    // first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i; //sets virtual page
        pageTable[i].physicalPage = memoryManager->AllocPage();  //physical

        /*NAN CHEN*/
        //The "physPages" bitmap of memoryManager is of size "NumPhysPages" 
        //  (as initialized in protest.cc).
        //
        //Everytime we call Exec(), Exec() will do something similar to 
        //  StartProcess() in progtest.cc (see write-up part2). Therefore, 
        //  This constructor of AddrSpace will be called in Exec.
        //
        //What we want is that, when AddrSpace finds out that there is not
        //  enough enough physical memory, in which case that
        //  memoryManager->AllocPage() == -1, AddrSpace will do something
        //  to stop allocating the pages and stop the process. 
        //
        //Since the ctor of AddrSpace is called in Exec(), we will return
        //  an unusual flag (in this case, -1) to Exec(), which Exec() will
        //  recognize and then stop the process.
        //
        //When Exec() is stopping the process, it will probably delete
        //  the AddrSpace. In C++, delete keyword calls the destructor of
        //  the class. 
        //
        //THEREFORE, in this for loop, we simply return -1, LEAVING THE JOB
        //  OF DEALLOCATING AND DELETING THE PageTable TO THE DESTRUCTOR of
        //  AddrSpace
        //
        if (pageTable[i].physicalPage == -1) 
	{
           printf ("addrSpace.cc: Not enough physical page available\n");
           for (unsigned int j = 0; j <= i; ++j) {
              memoryManager->FreePage(pageTable[j].physicalPage);
           }
           return -1;
        }
        /*NAN CHEN*/

        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only
    }

    //loop through each virtual page and (2) zero out that page.
    //bzero(machine->mainMemory, size);
    for (i = 0; i < numPages; i++) {
        //get a pointer to the physical page by mapping the virtual page
        //to the physical page
  	//zero the physical page using the pointer to it;
      memset(&machine->mainMemory[pageTable[i].physicalPage * PageSize], 0, PageSize);
   }

   // then, copy in the code and data segments into memory
   unsigned int file_off = noffH.code.inFileAddr;
   unsigned int virt_addr = noffH.code.virtualAddr;
   unsigned int end_virt_addr = noffH.code.virtualAddr + noffH.code.size;
   char *phys_addr;
   
   if(noffH.code.size > 0)
   {
       //if the section start address is not on a page boundary
       if(virt_addr % PageSize != 0)
       {
          phys_addr = vir_to_phys(virt_addr);

	  //This section only exists in one section
	  if(virt_addr/PageSize == (end_virt_addr)/PageSize)
	  {
	     executable->ReadAt(phys_addr, noffH.code.size, file_off);
             file_off += noffH.code.size;
	     virt_addr += noffH.code.size;
	  }
	  else  //this section lasts in more than 2 page
	  {
             executable->ReadAt(phys_addr, PageSize - virt_addr % PageSize, file_off);
    	     file_off += PageSize - virt_addr % PageSize;
             virt_addr += PageSize - virt_addr % PageSize;
	  }
       }

       //load a full page when the remaining lasts for more than 1 page
       while (virt_addr + PageSize <= end_virt_addr) {
          //convert the virt_addr to a physical address using your page table;
          phys_addr = vir_to_phys(virt_addr);

          executable->ReadAt(phys_addr, PageSize, file_off);

	  //update current offset into executable file
          file_off += PageSize;

          //update the virtual address that the code gets loaded at;
          virt_addr += PageSize;          
       }

       //if the end of the segment is not a full page, load the remaining fragment
       if(virt_addr < end_virt_addr)
       {
          //on boundary
          phys_addr = vir_to_phys(virt_addr);

          executable->ReadAt(phys_addr, end_virt_addr - virt_addr, file_off);
       }
    }

   file_off = noffH.initData.inFileAddr;
   virt_addr = noffH.initData.virtualAddr;
   end_virt_addr = noffH.initData.virtualAddr + noffH.initData.size;

   if(noffH.initData.size > 0)
   {
       //if the section start address is not on a page boundary
       if(virt_addr % PageSize != 0)
       {
          phys_addr = vir_to_phys(virt_addr);

	  //This section only exists in one section
	  if(virt_addr/PageSize == (end_virt_addr)/PageSize)
	  {
	     executable->ReadAt(phys_addr, noffH.initData.size, file_off);
             file_off += noffH.initData.size;
	     virt_addr += noffH.initData.size;
	  }
	  else  //this section lasts in more than 2 page
	  {
             executable->ReadAt(phys_addr, PageSize - virt_addr % PageSize, file_off);
    	     file_off += PageSize - virt_addr % PageSize;
             virt_addr += PageSize - virt_addr % PageSize;
	  }
       }

       //load a full page when the remaining lasts for more than 1 page
       while (virt_addr + PageSize <= end_virt_addr) {
          //convert the virt_addr to a physical address using your page table;
          phys_addr = vir_to_phys(virt_addr);

          executable->ReadAt(phys_addr, PageSize, file_off);

	  //update current offset into executable file
          file_off += PageSize;

          //update the virtual address that the code gets loaded at;
          virt_addr += PageSize;          
       }

       //if the end of the segment is not a full page, load the remaining fragment
       if(virt_addr < end_virt_addr)
       {
          //on boundary
          phys_addr = vir_to_phys(virt_addr);

          executable->ReadAt(phys_addr, end_virt_addr - virt_addr, file_off);
       }
    }

    return 0;

/*

    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
                           noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
                           noffH.initData.size, noffH.initData.inFileAddr);
    }
*/

}
