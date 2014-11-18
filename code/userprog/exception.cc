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
void incrementPC()
{
   machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
   machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
   machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}

static void syscallExit(int status) 
{
   printf("The result returned from EXIT: %d\n", status);

   currentThread->space->~AddrSpace();
   delete currentThread->space;
   currentThread->Finish();
   ASSERT(FALSE);
}

static SpaceId syscallExec(int name, int argc, int argv, int opt) {
   OpenFile *executable = fileSystem->Open(name);
   AddrSpace *space;

   //name is the virtual address where stores the filename
   //change to physical address to get filename string
   char *filename = new char[100];  //maximum size 100
   int i = 0; 
   int ch;
   for(int i = 0; i < 100; i++) 
   {
      int physaddr = &pageTable[name+i/PageSize]->physicalPage * PageSize + (name + i) % PageSize;
      ch = machine->mainMemory[physaddr];
      filename[i] = (char) ch;
      if(ch == 0)
         break;
   }

   //invalid name
   if(i == 99 && ch != 0)
   {
      printf("Invalid file name %s\n", name);
      return 0;
   }
 
   //check if the file is executable
   executable = fileSystem->Open(name);
   if(executable == NULL) {
      printf("Error, Unable to open file %s\n", name);
      return 0;
   }

   //create address space
   space = new AddrSpace(executable);
   space->Initialize();

   //create new thread
   Thread* newThread = new Thread(name);
   newThread->space = space;
   newThread->Fork(ProcessStart, space);

   delete executable;   // close file

   return spaceId;
}

void ProcessStart(AddrSpace* space) {
  space->InitRegisters();   // set the initial register values
  space->RestoreState();    // load page table register

  machine->Run();     // jump to the user progam
  ASSERT(FALSE);      // machine->Run never
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }
    else if((which == SyscallException) && (type == SC_Exit))
    {
	syscallExit(machine->ReadRegister(4));
    }
    else if((which == SyscallException) && (type == SC_Exec))
    {
	int spaceId = syscallExec(machine->ReadRegister(4),
	            		  machine->ReadRegister(5),
		    		  machine->ReadRegister(6),
		    		  machine->ReadRegister(7));
 	incrementPC();
	machine->WriteRegister(2, spaceId);
    }
    else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
