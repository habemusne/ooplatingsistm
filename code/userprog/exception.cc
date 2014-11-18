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



static void syscallExit(int status) 
{
   printf("The result returned from EXIT: %d\n", status);

   currentThread->space->~AddrSpace();
   currentThread->Finish();
   ASSERT(FALSE);
}

static SpaceId Exec(char *name, int argc, char **argv, int opt) {
  OpenFile *executable = fileSystem->Open(filename);
  AddrSpace *space;

  //check if the file is executable
  executable = fileSystem->Open(name);
  if(executable == NULL) {
    printf("Error, Unable to open file %s\n", name);
    currentThread->space->~AddrSpace();
    currentThread->Finish();
    ASSERT(FALSE);
    return;
  }

  //create address space
  space = new AddrSpace(executable);
  space->Initialize();

  //create new thread
  Thread* newThread = new Thread(name);
  newThread->space = space;
  newThread->Fork(ProcessStart, (int) space);


  delete executable;      // close file

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
	syscallExit( machine->ReadRegister(4));
    }
    else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}


