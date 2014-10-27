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


//-----------------------------------//
//Condition Variable Tests
//-----------------------------------//


//CondTest1: a thread waits the cond, another signals
Condition *cond1 = NULL;
Lock *LockTest5 = NULL;

void CondThread1(int param) {
  printf("CT1: acquiring");
  LockTest5->Acquire();
  printf("CT1: acquired. before wait");
  Cond1->Wait(LockTest5);
  printf("CT1: after wait");
  LockTest->Release();
  printf("CT1: released");
}

void CondThread2(int param) {
  // printf("CT1: acquiring");
  // LockTest5->Acquire();
  printf("CT2: acquired. before signal");
  Cond1->Signal(LockTest5);
  printf("CT2: after signal");
  // LockTest->Release();
  // printf("CT2: released");
}

void CondTest1()
{
    DEBUG('t', "Entering CondTest1");

    LockTest5 = new Lock("CondTest1_lock");
    Cond1 = new Condition("Cond1");


    Thread *t = new Thread("one");
    t->Fork(CondThread1, 0);

    t = new Thread("two");
    t->Fork(CondThread2, 0);
}


//CondTest2: two threads wait, one signal
void CondThread3(int param) {
  printf("CT3: acquiring");
  LockTest->Acquire();
  printf("CT3: acquired. before wait");
  Cond1->Wait(LockTest5);
  printf("CT3: after wait");
  LockTest->Release();
  printf("CT3: released");
}

void CondThread4(int param) {
  printf("CT4: acquiring");
  LockTest5->Acquire();
  printf("CT4: acquired. before wait");
  Cond1->Wait(LockTest5);
  printf("CT4: after wait");
  LockTest->Release();
  printf("CT4: released");
}

void CondThread5(int param) {
  // printf("CT5: acquiring");
  // LockTest5->Acquire();
  printf("CT5: acquired. before signal");
  Cond1->Signal(LockTest5);
  printf("CT5: after signal");
  // LockTest->Release();
  // printf("CT5: released");
}

void CondTest2()
{
    DEBUG('t', "Entering CondTest2");

    LockTest5 = new Lock("CondTest2_lock");
    Cond1 = new Condition("Cond1");

    Thread *t = new Thread("three");
    t->Fork(CondThread3, 0);

    Thread *t = new Thread("four");
    t->Fork(CondThread4, 0);

    t = new Thread("five");
    t->Fork(CondThread5, 0);
}


//ConTest3: waiting on a condition variable without holding lock
void CondThread6(int param) {
    Cond1->wait(LockTest5);
    LockTest5->Release();
}

void CondTest3() {
    DEBUG('t', "Entering CondTest3");
    LockTest5 = new Lock("CondTest3_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("six");
    t->Fork(CondThread6, 0);

}


//CondTest4: broadcast wakes up all threads
void CondThread7(int param) {
  printf("CT7: acquiring");
  LockTest5->Acquire();
  printf("CT7: acquired. before signal");
  Cond1->Wait(LockTest5);
  printf("CT7: after broadcasting");
  LockTest5->Release();
  printf("CT7: after releasing");
}

void CondThread8(int param) {
  printf("CT8: acquiring");
  LockTest5->Acquire();
  printf("CT8: acquired. before signal");
  Cond1->Wait(LockTest5);
  printf("CT8: after broadcasting");
  LockTest5->Release();
  printf("CT8: after releasing");
}

void CondThread9(int param) {
  // printf("CT9: acquiring");
  // LockTest5->Acquire();
  printf("CT9: signaling");
  Cond1->Broadcast(LockTest5);
  printf("CT9: signaled");
  // printf("CT9: after broadcasting");
  // LockTest5->Release();
  // printf("CT9: after releasing");
}

void CondTest4() {
    DEBUG('t', "Entering CondTest4");
    LockTest5 = new Lock("CondTest4_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("seven");
    t->Fork(CondThread7, 0);
    Thread *t = new Thread("eight");
    t->Fork(CondThread8, 0);
    Thread *t = new Thread("nine");
    t->Fork(CondThread9, 0);
}

//CondTest5: signaling to a condition variable with no waiters is a no-op, and future threads that wait will block
void CondThread10(int param) {
    // printf("CT10: acquiring");
    // LockTest5->Acquire();
    printf("CT10: signaling");
    Cond1->Signal(LockTest5);
    printf("CT10: signaled");
    // printf("CT10: releasing");
    // LockTest5->Release();
    // printf("CT10: released");
}

void CondThread11(int param) {
    printf("CT11: acquiring");
    LockTest5->Acquire();
    printf("CT11: waiting");
    Cond1->Wait(LockTest5);
    printf("CT11: releasing");
    LockTest5->Release();
    printf("CT11: released");
}

void CondThread12(int param) {
    printf("CT12: acquiring");
    LockTest5->Acquire();
    printf("CT12: waiting");
    Cond1->Wait(LockTest5);
    printf("CT12: releasing");
    LockTest5->Release();
    printf("CT12: released");
}

void CondTest5() {
    DEBUG('t', "Entering CondTest5");
    LockTest5 = new Lock("CondTest5_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("ten");
    t->Fork(CondThread10, 0);
    Thread *t = new Thread("eleven");
    t->Fork(CondThread11, 0);
    Thread *t = new Thread("twelve");
    t->Fork(CondThread12, 0);
}

//CondTest6: broadcasting to a condition variable with no waiters is a no-op, and future threads that wait will block
void CondThread13(int param) {
    // printf("CT13: acquiring");
    // LockTest5->Acquire();
    printf("CT13: broadcasting");
    Cond1->Broadcast(LockTest5);
    printf("CT13: broadcasted");
    // printf("CT13: releasing");
    // LockTest5->Release();
    // printf("CT13: released");
}

void CondThread14(int param) {
    printf("CT14: acquiring");
    LockTest5->Acquire();
    printf("CT14: waiting");
    Cond1->Wait(LockTest5);
    printf("CT14: releasing");
    LockTest5->Release();
    printf("CT14: released");
}

void CondThread15(int param) {
    printf("CT15: acquiring");
    LockTest5->Acquire();
    printf("CT15: waiting");
    Cond1->Wait(LockTest5);
    printf("CT15: releasing");
    LockTest5->Release();
    printf("CT15: released");
}

void CondTest6() {
    DEBUG('t', "Entering CondTest6");
    LockTest5 = new Lock("CondTest6_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("thirteen");
    t->Fork(CondThread13, 0);
    Thread *t = new Thread("fourteen");
    t->Fork(CondThread14, 0);
    Thread *t = new Thread("fifteen");
    t->Fork(CondThread15, 0);
}

//TODO
//CondTest7: a thread calling Signal holds the Lock passed in to Signal
void CondThread16(int param) {
    printf("CT16: acquiring");
    LockTest5->Acquire();
    printf("CT16: waiting");
    Cond1->Signal(LockTest5);
    printf("CT16: releasing");
    LockTest5->Release();
    printf("CT16: released");
}

void CondTest7() {
    DEBUG('t', "Entering CondTest7");
    LockTest5 = new Lock("CondTest7_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("sixteen");
    t->Fork(CondThread16, 0);
}

//CondTest8: deleting a lock should have no threads on the wait queue
void CondThread17(int param) {
    printf("CT17: acquiring");
    LockTest5->Acquire();
    printf("CT17: waiting");
    Cond1->Wait(LockTest5);
    printf("CT17: releasing");
    LockTest5->Release();
    printf("CT17: released");
}

void CondThread18(int param) {
    printf("CT18: deleting lock");
    LockTest5->~Lock();
    printf("CT18: lock deleted");
}

void CondTest8() {
    DEBUG('t', "Entering CondTest8");
    LockTest5 = new Lock("CondTest8_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("seventeen");
    t->Fork(CondThread17, 0);
    Thread *t = new Thread("eighteen");
    t->Fork(CondThread18, 0);
}


//CondTest9: deleting a condition variable should have no threads on the wait queue
void CondThread19(int param) {
    printf("CT19: acquiring");
    LockTest5->Acquire();
    printf("CT19: waiting");
    Cond1->Wait(LockTest5);
    printf("CT19: releasing");
    LockTest5->Release();
    printf("CT19: released");
}

void CondThread20(int param) {
    printf("CT20: deleting CV");
    Cond1->~Condition();
    printf("CT20: CV deleted");
}

void CondTest9() {
    DEBUG('t', "Entering CondTest9");
    LockTest5 = new Lock("CondTest9_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("ninteen");
    t->Fork(CondThread19, 0);
    Thread *t = new Thread("twenty");
    t->Fork(CondThread20, 0);
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
	    CondTest1();
	break;
    default:
        printf("No test specified.\n");
        break;
    }
}

