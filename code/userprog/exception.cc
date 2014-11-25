// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "table.h"
#include "memorymanager.h"
#include "machine.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

// increment the PC
static void incrementPC()
{
   machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
   machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
   
   /*NAN CHEN*/
   //I am sure this is correct. should be incrented by 4
   machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
   //machine->WriteRegister(NextPCReg, PrevPCReg);
   /*NAN CHEN*/
}

static void syscallExit(int status) 
{
   printf("[syscallExit] Thread %s is exiting\n", currentThread->getName());
   printf("[syscallExit] The result returned from EXIT: %d\n", status);

   //do not do ~space again, delete call destructor
   delete currentThread->space;

   table->Release(table->GetIndex(currentThread));
   currentThread->Finish();
   ASSERT(FALSE);
}

static void ProcessStart(AddrSpace *space) {
  space->InitRegisters();   // set the initial register values
  space->RestoreState();    // load page table register

  machine->Run();     // jump to the user progam
  ASSERT(FALSE);      // machine->Run never
}

static SpaceId syscallExec(int name, int argc, int argv, int opt) {
   OpenFile *executable;
   AddrSpace *space;
   printf("[syscallExec] In syscallExec()\n");

   //name is the virtual address where stores the filename
   //change to physical address to get filename string
   char *filename = new char[100];  //maximum size 100
   int i = 0; 
   int ch;
   for(i = 0; i < 100; i++) 
   {
      //int physaddr = (pageTable[(name + i) / PageSize]->physicalPage) * PageSize + (name + i) % PageSize;
      char *physicalAddress = currentThread->space->vir_to_phys(name + i);
      ch = *physicalAddress;
      printf("%c", (char)ch);
      filename[i] = (char) ch;
      if(ch == 0)
         break;
   }

   printf("\n");
   //invalid name
   /*NAN CHEN: Changed i == 99 to i >= 99*/
   if(i >= 99 && ch != 0)
   {
      printf("Invalid file name %s\n", filename);
      return 0;
   }
 
   //check if the file is executable
   executable = fileSystem->Open(filename);
   if(executable == NULL) {
      printf("Error, Unable to open file %s\n", filename);
      return 0;
   }

   /*NAN CHEN*/
   char* argument = new char[100];
   i = 0;
   ch = 0;
   for (i = 0; i < 100; ++i){
     char *physicalAddress = currentThread->space->vir_to_phys(argv + i);
     ch = *physicalAddress;
     argument[i] = (char) ch;
     printf("%c", (char)ch);
     if (ch == 0){
       break;
     }
   }
   printf("\n");
   if (i >= 99 && ch != 0){
     printf("Invalid argument %s\n", argument);
     return 0;
   }
   /*NAN CHEN*/

   //create address space
   space = new AddrSpace(executable);

   /*NAN CHEN*/
   //space->Initialize(executable);
   int spaceReturn = space->Initialize(executable, false, (char*)argv, i);
   if (spaceReturn == -1){
     printf("Error, unable to initialize address space for %s \n", filename);
     return 0;
   }
   delete argument;
   /*NAN CHEN*/

   //create new thread
   Thread* newThread = new Thread(filename);
   int spaceId = table->Alloc(newThread);
   newThread->space = space;
   newThread->Fork((VoidFunctionPtr) ProcessStart, (int) newThread->space);

   delete executable;   // close file

   printf("[syscallExec] The spaceID of %s is %d\n", newThread->getName(), spaceId);
   return spaceId;
}


static int syscallRead(int buffer_addr, int size, int id) {
   
   if(size == 0)
      return 0;

   if(id == 0)
      return synchConsole->SynchGetString(buffer_addr, size);
   else
      printf("Read from a file has not been impelemented %d\n", id);

   return -1;   //the number of bytes actually read
}

static void syscallWrite(int buffer_addr, int size, int id) {
   //printf("[syscallWrite] buffer_addr is %d, and size is %d\n", buffer_addr, size);

   if(size != 0)
   { 
      if(id == 1){
	 //printf("enter.....................\n");
         synchConsole->SynchPutString(buffer_addr, size);
      }
      else
         printf("Write to a file has not been impelemented %d\n", id);
	 
   }
}

static void pageFaultHandler()
{
   //the failing virtual address on an exception
   ++stats->numPageFaults;
   unsigned int faultAddress = machine->ReadRegister(BadVAddrReg);

   unsigned int vpn = (unsigned)badAddr/PageSize;
   int physPage = memoryManager->AllocPage();


   pageTable[vpn].physicalPage = physPage;
   pageTable[vpn].valid = true;
   swapFile->ReadAt(&(machine->mainMemory[physPage*PageSize]), PageSize, pageTable[vpn].swapPage * PageSize);

}



void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
        DEBUG('a', "Shutdown, initiated by user program.\n");
	printf("[exceptionHandler] SC_Halt\n");

        interrupt->Halt();
    }
    else if((which == SyscallException) && (type == SC_Exit))
    {
    	printf("[exceptionHandler] SC_Exit\n");

	syscallExit(machine->ReadRegister(4));
    }
    else if((which == SyscallException) && (type == SC_Exec))
    {
    	printf("[exceptionHandler] SC_Exec\n");

	int spaceId = syscallExec(machine->ReadRegister(4),
	            		  machine->ReadRegister(5),
		    		  machine->ReadRegister(6),
		    		  machine->ReadRegister(7));
 	incrementPC();
	machine->WriteRegister(2, spaceId);
    }
    else if((which == SyscallException) && (type == SC_Read))
    {
       
       int read_num;
       read_num = syscallRead(machine->ReadRegister(4),
	               	      machine->ReadRegister(5),
		              machine->ReadRegister(6));
       incrementPC();
       machine->WriteRegister(2, read_num);
       
    }
    else if((which == SyscallException) && (type == SC_Write))
    {
       
       syscallWrite(machine->ReadRegister(4),
	           machine->ReadRegister(5),
		   machine->ReadRegister(6));
       incrementPC();
       machine->WriteRegister(2, 0);
       
    }
    else if(which == NoException)
    {

    }
    else if(which == PageFaultException)
    {
       //DO not increment PC
       //Run current instruction again
       pageFaultHandler();

    }
    else if(which == ReadOnlyException)
    {

    }
    else if(which == BusErrorException)
    {

    }
    else if(which == AddressErrorException)
    {

    }
    else if(which == OverflowException)
    {

    }
    else if(which == IllegalInstrException)
    {

    }
    else if(which == NumExceptionException)
    {

    }
    else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
