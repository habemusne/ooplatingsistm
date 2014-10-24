// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// LockTest1
//----------------------------------------------------------------------

Lock *locktest1 = NULL;
Lock *locktest2 = NULL;
Lock *locktest3 = NULL;
Lock *locktest4 = NULL;


void
LockThread1(int param)
{
    printf("L1:0\n");
    locktest1->Acquire();
    printf("L1:1\n");
    currentThread->Yield();
    printf("L1:2\n");
    locktest1->Release();
    printf("L1:3\n");
}

void
LockThread2(int param)
{
    printf("L2:acquire\n");
    locktest2->Acquire();
    printf("L2:acquire\n");
    locktest2->Acquire();
    printf("L2:1\n");
    currentThread->Yield();
    printf("L2:2\n");
    locktest2->Release();
    printf("L2:3\n");
}

void LockThread3(int param)
{
    printf("L3:release\n");
    locktest3->Release();
}

void LockThread4(int param)
{
    printf("L4:acquire");
    locktest4->Acquire();
    printf("L4:delete\n");
    locktest4->~Lock();
}


void
LockTest1()
{
    DEBUG('t', "Entering LockTest1");

    locktest1 = new Lock("LockTest1");

    Thread *t = new Thread("one");
    t->Fork(LockThread1, 0);
}

//test2:acquiring the same lock twice
void LockTest2()
{
    DEBUG('t', "Entering LockTest2");

    locktest2 = new Lock("LockTest2");

    Thread *t = new Thread("two");
    t->Fork(LockThread2, 0);
}

//test3
void LockTest3()
{
    DEBUG('t', "Entering LockTest3");

    locktest3 = new Lock("LockTest3");

    Thread *t = new Thread("three");
    t->Fork(LockThread3, 0);

}

//test4
void CondThread1(int param) {
  
}

void LockTest4()
{
    DEBUG('t', "Entering LockTest4");

    locktest4 = new Lock("LockTest4");

    Thread *t = new Thread("four");
    t->Fork(CondThread1, 0);

}


//-----------------------------------//
//Condition Variable Tests
//-----------------------------------//

Condition cond1 = NULL;
Condition cond2 = NULL;

//CV Test 1
void CVTest1() {
  DEBUG('t', "Entering CVTest1");
  cond1 = new Condition("Cond1");
  Thread *t = new Thread("five");
  t->Fork();
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 0:
	   ThreadTest1();
	break;
    case 1:
	    LockTest1();
	break;
    case 2:
	    LockTest2();
	break;
    case 3:
	    LockTest3();
	break;
    case 4:
	    LockTest4();
	break;
    case 5:
      CVTest1();

    default:
        printf("No test specified.\n");
        break;
    }
}

