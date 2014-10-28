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

//Aquiring the same Lock twice
void
LockThread2(int param)
{
    printf("L2:acquire\n");
    locktest2->Acquire();
    printf("L2:acquire\n");
    locktest2->Acquire();
    printf("L2:1 [UNEXPECTED] \n");
    currentThread->Yield();
    printf("L2:2\n");
    locktest2->Release();
    printf("L2:3\n");
}

//release a lock that isn't held
void LockThread3(int param)
{
    printf("L3:release\n");
    locktest3->Release();
}

//deleting a Lock that is held
void LockThread4(int param)
{
    printf("L4:acquire\n");
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

void LockTest3()
{
    DEBUG('t', "Entering LockTest3");

    locktest3 = new Lock("LockTest3");

    Thread *t = new Thread("three");
    t->Fork(LockThread3, 0);
}

void LockTest4()
{
    DEBUG('t', "Entering LockTest4");

    locktest4 = new Lock("LockTest3");

    Thread *t = new Thread("four");
    t->Fork(LockThread4, 0);
}




//-----------------------------------//
//Condition Variable Tests
//-----------------------------------//


//CondTest1: a thread waits the cond, another signals
Condition *Cond1 = NULL;
Lock *LockTest5 = NULL;

void CondThread1(int param) {
  printf("CT1: acquiring");
  LockTest5->Acquire();
  printf("CT1: acquired. before wait");
  Cond1->Wait(LockTest5);
  printf("CT1: after wait");
  LockTest5->Release();
  printf("CT1: released");
}

void CondThread2(int param) {
  // printf("CT1: acquiring");
  // LockTest5->Acquire();
  printf("CT2: acquired. before signal");
  Cond1->Signal(LockTest5);
  printf("CT2: after signal");
  // LockTest5->Release();
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
  LockTest5->Acquire();
  printf("CT3: acquired. before wait");
  Cond1->Wait(LockTest5);
  printf("CT3: after wait");
  LockTest5->Release();
  printf("CT3: released");
}

void CondThread4(int param) {
  printf("CT4: acquiring");
  LockTest5->Acquire();
  printf("CT4: acquired. before wait");
  Cond1->Wait(LockTest5);
  printf("CT4: after wait");
  LockTest5->Release();
  printf("CT4: released");
}

void CondThread5(int param) {
  // printf("CT5: acquiring");
  // LockTest5->Acquire();
  printf("CT5: acquired. before signal");
  Cond1->Signal(LockTest5);
  printf("CT5: after signal");
  // LockTest5->Release();
  // printf("CT5: released");
}

void CondTest2()
{
    DEBUG('t', "Entering CondTest2");
printf("haha");
    LockTest5 = new Lock("CondTest2_lock");
    Cond1 = new Condition("Cond1");

    Thread *t = new Thread("three");
    t->Fork(CondThread3, 0);

    Thread *t1 = new Thread("four");
    t1->Fork(CondThread4, 0);

    t = new Thread("five");
    t->Fork(CondThread5, 0);
}


//ConTest3: waiting on a condition variable without holding lock
void CondThread6(int param) {
    Cond1->Wait(LockTest5);
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
    Thread *t1 = new Thread("eight");
    t1->Fork(CondThread8, 0);
    Thread *t2 = new Thread("nine");
    t2->Fork(CondThread9, 0);
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
    Thread *t1 = new Thread("eleven");
    t1->Fork(CondThread11, 0);
    Thread *t2 = new Thread("twelve");
    t2->Fork(CondThread12, 0);
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
    Thread *t1 = new Thread("fourteen");
    t1->Fork(CondThread14, 0);
    Thread *t2 = new Thread("fifteen");
    t2->Fork(CondThread15, 0);
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
    Thread *t1 = new Thread("eighteen");
    t1->Fork(CondThread18, 0);
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
    Thread *t1 = new Thread("twenty");
    t1->Fork(CondThread20, 0);
}


//-----------------------------------//
//Mailbox Tests
//-----------------------------------//
/*
int m = 1;
int m1 = 2;
Mailbox * mailbox1 = NULL;
void MailboxThread1(int param) {
   mailbox1->Send(m);
   printf("Mailbox1:send");
}

void MailboxThread2(int param) {
   mailbox1->Receive(&m);
   printf("Mailbox1:receive");
}

//same mailbox: first send then receive
void MTest1(){
  DEBUG('t', "Entering MTest1");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1,0);
  Thread *t1 = new Thread("mthread 2");
  t1->Fork(MailboxThread2,0);
}

Mailbox * mailbox2 = NULL;
void MailboxThread3(int param){
  mailbox2->Receive(&m1);
  printf("Mailbox2:receive");
}

void MailboxThread4(int param){
  mailbox2->Send(m1);
  printf("Mailbox2:send");
}

//diff mailbox: first send then receive
void MTest2(){
  DEBUG('t', "Entering MTest2");
  
  mailbox1 = new Mailbox("Mailbox1");
  mailbox2 = new Mailbox("Mailbox2");
  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1,0);
  Thread *t1 = new Thread("mthread 3");
  t1->Fork(MailboxThread3,0);
}


//same mailbox: first receive then send
void MTest3(){
  DEBUG('t', "Entering MTest3");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1,0);
  Thread *t1 = new Thread("mthread 2");
  t1->Fork(MailboxThread2,0);
}

//diff mailbox: first receive then send
void MTest4(){
  DEBUG('t', "Entering MTest4");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 3");
  t->Fork(MailboxThread3,0);
  Thread *t1 = new Thread("mthread 1");
  t1->Fork(MailboxThread1,0);
}

//same mailbox: ssrr
void MTest5(){
  DEBUG('t', "Entering MTest5");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1,0);
  Thread *t1 = new Thread("mthread 1");
  t1->Fork(MailboxThread1,0);
  Thread *t2 = new Thread("mthread 2");
  t2->Fork(MailboxThread2,0);
  Thread *t3 = new Thread("mthread 2");
  t3->Fork(MailboxThread2,0);
}

//diff mailbox: ssrr
void MTest6(){
  DEBUG('t', "Entering MTest6");
  
  mailbox1 = new Mailbox("Mailbox1");
  mailbox2 = new Mailbox("Mailbox2");

  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1,0);
  Thread *t1 = new Thread("mthread 4");
  t1->Fork(MailboxThread4,0);
  Thread *t2 = new Thread("mthread 2");
  t2->Fork(MailboxThread2,0);
  Thread *t3 = new Thread("mthread 3");
  t3->Fork(MailboxThread3,0);
}

void MTest7(){
  DEBUG('t', "Entering MTest7");
  
  mailbox1 = new Mailbox("Mailbox1");
  mailbox2 = new Mailbox("Mailbox2");

  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1,0);
  Thread *t1 = new Thread("mthread 4");
  t1->Fork(MailboxThread4,0);
  Thread *t2 = new Thread("mthread 3");
  t2->Fork(MailboxThread3,0);
  Thread *t3 = new Thread("mthread 2");
  t3->Fork(MailboxThread2,0);
}

//one send
void MTest8(){
  DEBUG('t', "Entering MTest8");
  
  mailbox1 = new Mailbox("Mailbox1");

  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1,0);
}

//one receive
void MTest9(){
  DEBUG('t', "Entering MTest9");
  
  mailbox1 = new Mailbox("Mailbox1");

  Thread *t = new Thread("mthread 2");
  t->Fork(MailboxThread2,0);
}
*/

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
	    CondTest2();
	break;
    case 6:
	    CondTest3();
	break;
    case 7:
	    CondTest4();
	break;
    case 8:
	    CondTest5();
	break;
    case 9:
	    CondTest6();
	break;
    case 10:
	    CondTest7();
	break;
    case 11:
	    CondTest8();
	break;
    case 12:
	    CondTest9();
	break;
    case 13:
	    //MTest1();
	break;

    default:
        printf("No test specified.\n");
        break;
    }
}

