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
#ifdef HOST_SPARC
#include <strings.h>
#endif
#include "table.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

//bool WriteMem(int addr, int size, int value);

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

AddrSpace::AddrSpace(OpenFile *executableFile)
{
  /*NAN CHEN*/
  //
  //All previous code is moved to Initialize() method. In Exec(), Exec()
  //  needs to firstly call this constructor, and then call 
  //  addrSpace->Initialize();
  //
  this->numPages = 0;
  this->pageTable = NULL;
  this->executable = NULL;
  //this->argument_size = 0;
  //this->argument_addr =0;
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
  delete executable;
  //delete [] argument_addr;
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



int AddrSpace::Initialize(OpenFile *executableFile, bool isProgTest){

    //this->argument_size = argumentSize;
   // this->argument_addr = arg_vird;

    unsigned int i, size; 
    this->executable = executableFile;

    this->executable->ReadAt((char *)(&noffH), sizeof(noffH), 0);
    if ((this->noffH.noffMagic != NOFFMAGIC) &&
            (WordToHost(this->noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(this->noffH.noffMagic == NOFFMAGIC);

    // how big is address space?
    //size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
    //       + UserStackSize + argumentSize; // we need to increase the size
    size = this->noffH.code.size + this->noffH.initData.size
         + this->noffH.uninitData.size + UserStackSize;

    // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
printf("numPages = %d\n", numPages);

    //if it is the main thread
    if(isProgTest)
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

/*
        if (pageTable[i].physicalPage == -1) 
	{
           printf ("ERROR MESSAGE: addrSpace.cc: No enough physical page available\n");
           for (unsigned int j = 0; j < i; ++j) {
              memoryManager->FreePage(pageTable[j].physicalPage);
           }
           return -1;
        }
*/
        pageTable[i].valid = FALSE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
        // a separate page, we could set its
        // pages to be read-only
    }

/*
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

          file_off += end_virt_addr - virt_addr;
       }
    }
*/
/*
printf("argumentSize = %d\n", argumentSize);
   if(argumentSize > 0)
   {
       char *charArrPtr = (char*)machine->ReadRegister(6);
printf("addrSpace.cc: charArrPtr = %d\n", charArrPtr);
       virt_addr = (unsigned int)(charArrPtr);
       end_virt_addr = virt_addr + argumentSize;
       for (int j = 0; j < argumentSize; ++j){
         int value = (int)(*(char*)(virt_addr));
         machine->WriteMem(virt_addr, 1, value);
         virt_addr += 1;
       }
       //if the section start address is not on a page boundary
       char **charArrPtr = (char**)machine->ReadRegister(6);
       char* charArr = *charArrPtr;
       virt_addr = (unsigned int)(charArrPtr);
       end_virt_addr = virt_addr + argumentSize;
       if(virt_addr % PageSize != 0)
       {
	  //This section only exists in one section
	  if(virt_addr/PageSize == (end_virt_addr)/PageSize)
	  {
             for (unsigned int j = 0; j < (unsigned int)argumentSize; ++j){
               int k = (int)(*(char*)(virt_addr));
               machine->WriteMem(virt_addr, 1, k);
               virt_addr += 1;
             }
	     virt_addr += argumentSize;
	  }
          else  //this section lasts in more than 2 page
	  {
             for (unsigned int j = 0; j < PageSize - virt_addr % PageSize; ++j){
               int k = (int) (*(char*)(virt_addr));
               machine->WriteMem(virt_addr, 1, k);
    	       virt_addr += 1;
             }
	  }
       }
       //load a full page when the remaining lasts for more than 1 page
       while (virt_addr + PageSize <= end_virt_addr) {
          //convert the virt_addr to a physical address using your page table;

          for (unsigned int j = 0; j < PageSize; ++j){
            int k = (int) (*(char*)(virt_addr));
            machine->WriteMem(virt_addr, 1, k);

	  //update current offset into executable file
            virt_addr += 1;
          }
       }

       //if the end of the segment is not a full page, load the remaining fragment
       if(virt_addr < end_virt_addr)
       {
          //on boundary
          for (unsigned int j = 0; j < end_virt_addr - virt_addr; ++j){
            int k = (int) (*(char*)(virt_addr));
            machine->WriteMem(virt_addr, 1, k);
            virt_addr += 1;
          }
       }
   }
*/
   return 0;
}

int AddrSpace::handlePageDemand(int faultAddr){

   unsigned int code_file_off = this->noffH.code.inFileAddr;
   unsigned int code_virt_addr = this->noffH.code.virtualAddr;
   unsigned int code_end_virt_addr = this->noffH.code.virtualAddr + this->noffH.code.size;
   
   unsigned int data_file_off = this->noffH.initData.inFileAddr;
   unsigned int data_virt_addr = this->noffH.initData.virtualAddr;
   unsigned int data_end_virt_addr = this->noffH.initData.virtualAddr + this->noffH.initData.size;

   int code_page_start = (int)code_virt_addr / PageSize;
   int code_page_end = (int)code_end_virt_addr / PageSize;
   int data_page_start = (int)data_virt_addr / PageSize;
   int data_page_end = (int)data_end_virt_addr / PageSize;
   int fault_page = faultAddr / PageSize;

   unsigned int load_start;
   unsigned int load_offset;
   unsigned int load_end;
   unsigned int load_at_position;

printf("code_file_off = %d, code_virt_addr %d, code_end_virt_addr = %d\n",
  code_file_off, code_virt_addr, code_end_virt_addr);
printf("data_file_off = %d, data_virt_addr = %d, dara_end_virt_addr = %d\n",
  data_file_off, data_virt_addr, data_end_virt_addr);
printf("code_page_start = %d, code_page_end = %d, ", code_page_start, code_page_end);
printf("data_page_start = %d, data_page_end = %d, ", data_page_start, data_page_end);
printf("fault_page = %d\n", fault_page);

   if ((code_page_start - code_page_end) + (data_page_start - data_page_end)
     > NumPhysPages || fault_page > NumPhysPages){
     printf("ERROR MESSAGE: not enough physical pages\n");
     return -1;
   }

   //If the faulting page locates at the page where code section ends and
   //  data section starts, load the remaining portion of code section and
   //  then load the first page of data
   if (fault_page == code_page_end && fault_page == data_page_start){
     load_start = PageSize * fault_page;
     load_offset = code_end_virt_addr % PageSize;
     load_end = load_start + load_offset;
     load_at_position = code_file_off + PageSize * code_page_end;

printf("If #1, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

     this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);

     //If there is initData, then load the initData. Otherwise, clear stack
     if (this->noffH.initData.size > 0){
       //If the data section starts and ends on the same page, then load 
       //  [data size] bits in the physical memory, starting from where
       //  the loading of code section ends
       if (data_page_start == data_page_end){
         load_start = load_end;
         load_offset = this->noffH.initData.size;
         load_end = load_start + load_offset;
         load_at_position = data_file_off;

printf("If #2, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

         this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);

         //If loading does not reach the page boundary, clear the stack
         if (load_end % PageSize != 0){
           
           load_start = load_end;
           if (load_start % PageSize > 0){
             load_offset = PageSize - load_start % PageSize;
             bzero(vir_to_phys(load_start), load_offset);
           } else {
             bzero(vir_to_phys(load_start), 0);
           }

printf("If #6, bzero_start = %d, bzero_offset = %d\n", load_start,
  load_offset);

         }
       }

       //If the data section lasts for more than one page, then just load
       //  until reaching the boundary of this page
       else {
         load_start = load_end;
         load_end = (data_page_start + 1) * PageSize;
         load_offset = load_end - load_start;
         load_at_position = data_file_off;

printf("Else #2, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

         this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);
       }
     }

     //Else, there is no initData. So probably part of the stack frame is here
     else {
       load_start = load_end;
       if (load_start % PageSize > 0){
         load_offset = PageSize - load_start % PageSize;

printf("Else #3, bzero_start = %d, bzero_offset = %d\n", load_start,
  load_offset);

         bzero(vir_to_phys(load_start), load_offset);
       } else {

printf("Else #4, bzero_start = %d, bzero_offset = %d\n", load_start,
  0);

         bzero(vir_to_phys(load_start), 0);
       }
     }
   }

   //If the faulting page locates at the page where data section ends and
   //  code section begins, then load everything of code and data into 
   //  this page
   else if (fault_page == data_page_end && fault_page == code_page_start){
     load_start = code_virt_addr;
     load_offset = this->noffH.code.size;
     load_at_position = code_file_off;

printf("If #3, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

     this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);

     load_start = data_virt_addr;
     load_offset = this->noffH.initData.size;
     load_at_position = data_file_off;

printf("If #3, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

     this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);
   }

   //If the faulting page locates at the code pages
   else if (fault_page >= code_page_start && fault_page <= code_page_end){

     //First case: code_page_start = code_page_end
     //Second case: fault_page == code_page_end
     //Both cases mean that either the whole code section or the remaining
     //  code section fits perfectly in one page.
     if (code_page_start == code_page_end || fault_page == code_page_end){
       load_start = code_virt_addr;
       load_offset = code_end_virt_addr - load_start;
       load_at_position = code_file_off + load_start;

printf("If #4, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

       this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);
     }

     //Third case: fault address is equal to code_virt_addr, and code_virt_addr
     //  does not start at page boundary
     else if (fault_page == code_page_start && code_virt_addr % PageSize != 0){
       load_start = code_virt_addr;
       load_offset = PageSize - code_virt_addr % PageSize;
       load_at_position = code_file_off;

printf("Else #5, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

       this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);
     }

     //General case: the code section starts at boundart and does not finish loading after this execution
     else {

       load_start = PageSize * fault_page;
       load_offset = PageSize;
       load_end = PageSize * (fault_page + 1);
       load_at_position = code_file_off + load_start;

printf("Else #6, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

       this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);
     }
   } 

   else if (fault_page >= data_page_start && fault_page <= data_page_end){

     if (data_page_start == data_page_end || fault_page == data_page_end){
       load_start = data_virt_addr;
       load_offset = data_end_virt_addr - load_start;
       load_end = data_end_virt_addr;
       load_at_position = data_file_off + load_start;

printf("If #5, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

       this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);

       //clear the remaining section, which might be belong to stack
       load_start = load_end;
       if (load_start % PageSize > 0){
         load_offset = PageSize - load_start % PageSize;
         bzero(vir_to_phys(load_start), load_offset);
       } else {
         bzero(vir_to_phys(load_start), 0);
       }

printf("If #6, bzero_start = %d, bzero_offset = %d\n", load_start,
  load_offset);

     }

     else if (fault_page == data_page_start && data_virt_addr % PageSize != 0){
       load_start = data_virt_addr;
       load_offset = PageSize - data_virt_addr % PageSize;
       load_at_position = data_file_off;

printf("Else #7, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

       this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);
     }

     else {
       load_start = PageSize * fault_page;
       load_offset = PageSize;
       load_end = PageSize * (fault_page + 1);
       load_at_position = data_file_off + load_start;

printf("Else #8, load_start = %d, load_offset = %d, load_pos = %d\n", load_start,
  load_offset, load_at_position);

       this->executable->ReadAt(vir_to_phys(load_start), load_offset, load_at_position);
     }
   }

   //Clear the stack frame
   else {
     load_start = fault_page * PageSize;
     load_offset = PageSize;
     bzero(vir_to_phys(load_start), load_offset);

printf("If #7, bzero_start = %d, bzero_offset = %d\n", load_start,
  load_offset);

   }


   printf("fault_page = %d\n", fault_page);
   //set the fault page to valid
   this->pageTable[fault_page].valid = TRUE;

printf("**PageDemand() Finished\n\n");

  return 0;
}
