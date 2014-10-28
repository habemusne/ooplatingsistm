// synch.cc
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

    while (value == 0) { 			// semaphore not available
        queue->Append((void *)currentThread);	// so go to sleep
        currentThread->Sleep();
    }
    value--; 					// semaphore available,
    // consume its value

    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments
// Note -- without a correct implementation of Condition::Wait(),
// the test case in the network assignment won't work!


//----------------------------------------------------------------------
// Lock::Lock
// 	Initialize a lock, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//----------------------------------------------------------------------

Lock::Lock(char* debugName) 
{
    this->held = 0;
    this->queue = new List;
    this->name = debugName;
    this->previous_thread = NULL;
}


//----------------------------------------------------------------------
// Lock::Lock
// 	De-allocate lock, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Lock::~Lock() 
{
    //Check if case (3) satisfies
    ASSERT(this->held == 0);

    delete this->name;
    delete this->queue;
    this->name = NULL;
    this->queue = NULL;
    this->previous_thread = NULL;
}


//----------------------------------------------------------------------
// Lock::Acquire
//	Wait until the lock is FREE, then set it to BUSY
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void Lock::Acquire() 
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

    //Check if case (1) satisfies
    ASSERT(this->previous_thread != (void *) currentThread);

    this->previous_thread = (void *)currentThread;
    while (held) { 			// semaphore not available
        queue->Append((void *)currentThread);	// so go to sleep
        currentThread->Sleep();
    }
    // consume its value
    held = 1;

    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Set lock to be FREE, waking up a thread waiting in Acquire
//	if necessary
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void Lock::Release() 
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    ASSERT(this->held == 1); //Check if case (2) satisfies

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    held = 0;
    (void) interrupt->SetLevel(oldLevel);


}


//----------------------------------------------------------------------
// Condition::Condition
// 	Initialize a condition, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//----------------------------------------------------------------------

Condition::Condition(char* debugName)
{
  this->name = debugName;
  this->queue = new List;
}

//----------------------------------------------------------------------
// Condition::~Condition
// 	De-allocate condition, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Condition::~Condition() 
{
  delete this->name;
  delete this->queue;
  this->name = NULL;
  this->queue = NULL;
}

//----------------------------------------------------------------------
// Condition::Wait
//	release the lock, relinquish the CPU until signaled, then
//	re-acquire the lock
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void Condition::Wait(Lock* conditionLock) 
{
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  conditionLock->Release();
  this->queue->Append((void *) currentThread);
  currentThread->Sleep();
  conditionLock->Acquire();
  (void) interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock* conditionLock) 
{
  Thread *thread;
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  thread = (Thread *)queue->Remove();
  if (thread != NULL)
    scheduler->ReadyToRun(thread);
  //else
    
  (void) interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock) 
{
  Thread* thread;
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  while (!queue->IsEmpty()) {
    thread = (Thread *) queue->Remove();
    if (thread != NULL)
      scheduler->ReadyToRun(thread);
  }
  (void) interrupt->SetLevel(oldLevel);
}

Mailbox::Mailbox(char* debugName)
{
  this->name = debugName;
  this->senderLock = new Lock("SenderLock");
  this->receiverLock = new Lock("ReceiverLock");
  this->cond = new Condition("condition");
}

Mailbox::~Mailbox()
{
  delete this->name;
  delete this->senderLock;
  delete this->receiverLock;
  delete this->cond;
  this->name = NULL;
  this->senderLock = NULL;
  this->receiverLock = NULL;
  this->cond = NULL;
}

void Mailbox::Send(int message)
{
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  senderLock->Acquire();
  cond->Wait(senderLock);
  cond->Signal(receiverLock);
  //copy message
  this->receiveBuffer = message;
  senderLock->Release();
  (void) interrupt->SetLevel(oldLevel);
}

void Mailbox::Receive(int * message)
{
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  receiverLock->Acquire();
  cond->Signal(senderLock);
  cond->Wait(receiverLock);
  //copy message
  this->receiveBuffer = *message;
  receiverLock->Release();
  (void) interrupt->SetLevel(oldLevel);
}
