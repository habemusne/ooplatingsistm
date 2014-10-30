// threadtest.cc
//  Simple test case for the threads assignment.
//
//  Create two threads, and have them context switch
//  back and forth between themselves by calling Thread::Yield,
//  to illustratethe inner workings of the thread system.
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
//  Loop 5 times, yielding the CPU to another ready thread
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.
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
//  Set up a ping-pong between two threads, by forking a thread
//  to call SimpleThread, and then calling SimpleThread ourselves.
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
    printf("(1)L1:0\n");
    locktest1->Acquire();
    printf("(2)L1:1\n");
    currentThread->Yield();
    printf("(3)L1:2\n");
    locktest1->Release();
    printf("(4)L1:3\n");
}

//Aquiring the same Lock twice
void
LockThread2(int param)
{
    printf("(1)L2:acquire\n");
    locktest2->Acquire();
    printf("(2)L2:acquire\n");
    locktest2->Acquire();
    printf("L2:1 [UNEXPECTED] \n");
    currentThread->Yield();
    printf("L2:2 [UNEXPECTED]\n");
    locktest2->Release();
    printf("L2:3 [UNEXPECTED]\n");
}

//release a lock that isn't held
void LockThread3(int param)
{
    printf("(1)L3:release\n");
    locktest3->Release();
    printf("L3:released [UNEXPECTED]\n");
}

//deleting a Lock that is held
void LockThread4(int param)
{
    printf("(1)L4:acquire\n");
    locktest4->Acquire();
    printf("(2)L4:delete\n");
    locktest4->~Lock();
    printf("L4:deleted [UNEXPECTED]\n");
}

void LockThread5(int param) {
  printf("(1)L5:acquire\n");
  locktest4->Acquire();
  printf("(2)L5:acquired\n");
}

void LockThread6(int param) {
  printf("(3)L6:release\n");
  locktest4->Release();
  printf("L6:released [UNEXPECTED]\n");
}

//test1: a thread acquires a lock, yields itself, then releases
void
LockTest1()
{
    DEBUG('t', "Entering LockTest1");

    locktest1 = new Lock("LockTest1");

    Thread *t = new Thread("one");
    t->Fork(LockThread1, 0);
}

//test2: acquiring the same lock twice
void LockTest2()
{
    DEBUG('t', "Entering LockTest2");

    locktest2 = new Lock("LockTest2");

    Thread *t = new Thread("two");
    t->Fork(LockThread2, 0);
}

//test3: release a lock that is not held
void LockTest3()
{
    DEBUG('t', "Entering LockTest3");

    locktest3 = new Lock("LockTest3");

    Thread *t = new Thread("three");
    t->Fork(LockThread3, 0);
}

//test4: deleting a lock that is held
void LockTest4()
{
    DEBUG('t', "Entering LockTest4");

    locktest4 = new Lock("LockTest4");

    Thread *t = new Thread("four");
    t->Fork(LockThread4, 0);
}

//test5: a thread releases the lock acquired by another thread
void LockTest5() {
  DEBUG('t', "Entering LockTest5");
  locktest4 = new Lock("LockTest5");
  Thread *t = new Thread("five");
  t->Fork(LockThread5, 0);
  t = new Thread("six");
  t->Fork(LockThread6, 0);
}


//-----------------------------------//
//Condition Variable Tests
//-----------------------------------//


//CondTest1: a thread waits the cond, another signals
Condition *Cond1 = NULL;
Lock *lockTest5 = NULL;

void CondThread1(int param) {
  printf("(1)CT1: acquiring\n");
  lockTest5->Acquire();
  printf("(2)CT1: acquired. before wait\n");
  Cond1->Wait(lockTest5);
  printf("(7)CT1: after wait\n");
  lockTest5->Release();
  printf("(8)CT1: released\n");
}

void CondThread2(int param) {
  printf("(3)CT2: acquiring\n");
  lockTest5->Acquire();
  printf("(4)CT2: acquired. before signal\n");
  Cond1->Signal(lockTest5);
  printf("(5)CT2: after signal\n");
  lockTest5->Release();
  printf("(6)CT2: released\n");
}

void CondTest1()
{
    DEBUG('t', "Entering CondTest1");

    lockTest5 = new Lock("CondTest1_lock");
    Cond1 = new Condition("Cond1");


    Thread *t = new Thread("one");
    t->Fork(CondThread1, 0);

    t = new Thread("two");
    t->Fork(CondThread2, 0);
}


//CondTest2: two threads wait, one signal
void CondThread3(int param) {
  printf("(1)CT3: acquiring\n");
  lockTest5->Acquire();
  printf("(2)CT3: acquired. before wait\n");
  Cond1->Wait(lockTest5);
  printf("(9)CT3: after wait\n");
  lockTest5->Release();
  printf("(10)CT3: released\n");
}

void CondThread4(int param) {
  printf("(3)CT4: acquiring\n");
  lockTest5->Acquire();
  printf("(4)CT4: acquired. before wait\n");
  Cond1->Wait(lockTest5);
  printf("CT4: after wait [UNEXPECTED]\n");
  lockTest5->Release();
  printf("CT4: released [UNEXPECTED]\n");
}

void CondThread5(int param) {
  printf("(5)CT5: acquiring\n");
  lockTest5->Acquire();
  printf("(6)CT5: acquired. before signal\n");
  Cond1->Signal(lockTest5);
  printf("(7)CT5: after signal\n");
  lockTest5->Release();
  printf("(8)CT5: released\n");
}

void CondTest2()
{
    DEBUG('t', "Entering CondTest2");
    lockTest5 = new Lock("CondTest2_lock");
    Cond1 = new Condition("Cond1");

    Thread *t = new Thread("three");
    t->Fork(CondThread3, 0);

    t = new Thread("four");
    t->Fork(CondThread4, 0);

    t = new Thread("five");
    t->Fork(CondThread5, 0);
}


//ConTest3: waiting on a condition variable without holding lock
void CondThread6(int param) {
    printf("(1)CT6:before wait\n");
    Cond1->Wait(lockTest5);
    printf("CT6':waited [UNEXPECTED]\n");
    lockTest5->Release();
    printf("CT6: released [UNEXPECTED]\n");
}

void CondTest3() {
    DEBUG('t', "Entering CondTest3");
    lockTest5 = new Lock("CondTest3_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("six");
    t->Fork(CondThread6, 0);

}

//broadcasting wakes up all threads,
void CondThread7(int param) {
  printf("(1)CT7: acquiring\n");
  lockTest5->Acquire();
  printf("(2)CT7: acquired. before signal\n");
  Cond1->Wait(lockTest5);
  printf("(9)CT7: after broadcasting\n");
  lockTest5->Release();
  printf("(10)CT7: after releasing\n");
}

void CondThread8(int param) {
  printf("(3)CT8: acquiring\n");
  lockTest5->Acquire();
  printf("(4)CT8: acquired. before signal\n");
  Cond1->Wait(lockTest5);
  printf("(11)CT8: after broadcasting\n");
  lockTest5->Release();
  printf("(12)CT8: after releasing\n");
}

void CondThread9(int param) {
  printf("(5)CT9: acquiring\n");
  lockTest5->Acquire();
  printf("(6)CT9: broadcasting\n");
  Cond1->Broadcast(lockTest5);
  printf("(7)CT9: broadcasted. releasing\n");
  lockTest5->Release();
  printf("(8)CT9: released\n");
}

void CondTest4() {
    DEBUG('t', "Entering CondTest4");
    lockTest5 = new Lock("CondTest4_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("seven");
    t->Fork(CondThread7, 0);
    t = new Thread("eight");
    t->Fork(CondThread8, 0);
    t = new Thread("nine");
    t->Fork(CondThread9, 0);
}

// CondTest5: signaling to a condition variable with no waiters is a no-op,
// and future threads that wait will block
void CondThread10(int param) {
    printf("(1)CT10: acquiring\n");
    lockTest5->Acquire();
    printf("(2)CT10: signaling\n");
    Cond1->Signal(lockTest5);
    printf("(3)CT10: signaled. releasing\n");
    lockTest5->Release();
    printf("(4)CT10: released\n");
}

void CondThread11(int param) {
    printf("(5)CT11: acquiring\n");
    lockTest5->Acquire();
    printf("(6)CT11: waiting\n");
    Cond1->Wait(lockTest5);
    printf("(13)CT11: releasing\n");
    lockTest5->Release();
    printf("(14)CT11: released\n");
}

void CondThread12(int param) {
    printf("(7)CT12: acquiring\n");
    lockTest5->Acquire();
    printf("(8)CT12: waiting\n");
    Cond1->Wait(lockTest5);
    printf("CT12: releasing [UNEXPECTED]\n");
    lockTest5->Release();
    printf("CT12: released [UNEXPECTED]\n");
}

void CondThread12_2(int param) {
  printf("(9)CT12_2:acquiring\n");
  lockTest5->Acquire();
  printf("(10)CT12_2:acquired. Signaling\n");
  Cond1->Signal(lockTest5);
  printf("(11)CT12_2:signaled. Releasing\n");
  lockTest5->Release();
  printf("(12)CT12_2:released\n");
}

void CondTest5() {
    DEBUG('t', "Entering CondTest5");
    lockTest5 = new Lock("CondTest5_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("ten");
    t->Fork(CondThread10, 0);
    t = new Thread("eleven");
    t->Fork(CondThread11, 0);
    t = new Thread("twelve");
    t->Fork(CondThread12, 0);
    t = new Thread("twelve_two");
    t->Fork(CondThread12_2, 0);
}

//CondTest6: broadcasting to a condition variable with no waiters is a no-op, 
//and future threads that wait will block
void CondThread13(int param) {
    printf("(1)CT13: acquiring\n");
    lockTest5->Acquire();
    printf("(2)CT13: broadcasting\n");
    Cond1->Broadcast(lockTest5);
    printf("(3)CT13: broadcasted. Releasing\n");
    lockTest5->Release();
    printf("(4)CT13: released\n");
}

void CondThread14(int param) {
    printf("(5)CT14: acquiring\n");
    lockTest5->Acquire();
    printf("(6)CT14: waiting\n");
    Cond1->Wait(lockTest5);
    printf("CT14: releasing [UNEXPECTED] \n");
    lockTest5->Release();
    printf("CT14: released [UNEXPECTED] \n");
}

void CondThread15(int param) {
    printf("(7)CT15: acquiring\n");
    lockTest5->Acquire();
    printf("(8)CT15: waiting\n");
    Cond1->Wait(lockTest5);
    printf("CT15: releasing [UNEXPECTED] \n");
    lockTest5->Release();
    printf("CT15: released [UNEXPECTED] \n");
}

void CondTest6() {
    DEBUG('t', "Entering CondTest6");
    lockTest5 = new Lock("CondTest6_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("thirteen");
    t->Fork(CondThread13, 0);
    t = new Thread("fourteen");
    t->Fork(CondThread14, 0);
    t = new Thread("fifteen");
    t->Fork(CondThread15, 0);
}

//CondTest7: a thread calling Signal holds the Lock passed in to Signal
void CondThread16(int param) {
    printf("(1)CT16: acquiring\n");
    lockTest5->Acquire();
    printf("(2)CT16: waiting\n");
    Cond1->Signal(lockTest5);
    printf("(3)CT16: releasing\n");
    lockTest5->Release();
    printf("(4)CT16: released\n");
}

void CondTest7() {
    DEBUG('t', "Entering CondTest7");
    lockTest5 = new Lock("CondTest7_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("sixteen");
    t->Fork(CondThread16, 0);
}

//CondTest8: deleting a lock should have no threads on the wait queue
void CondThread17(int param) {
    printf("(1)CT17: acquiring\n");
    lockTest5->Acquire();
    printf("(2)CT17: waiting\n");
    Cond1->Wait(lockTest5);
    printf("CT17: releasing [UNEXPECTED]\n");
    //lockTest5->Release();
    printf("CT17: released [UNEXPECTED]\n");
}

void CondThread18(int param) {
    printf("(3)CT18: deleting lock\n");
    lockTest5->~Lock();
    printf("CT18: lock deleted [UNEXPECTED]\n");
}

void CondTest8() {
    DEBUG('t', "Entering CondTest8");
    lockTest5 = new Lock("CondTest8_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("seventeen");
    t->Fork(CondThread17, 0);
    Thread *t1 = new Thread("eighteen");
    t1->Fork(CondThread18, 0);
}


//CondTest9: deleting a condition variable should have no threads on the wait queue
void CondThread19(int param) {
    printf("(1)CT19: acquiring\n");
    lockTest5->Acquire();
    printf("(2)CT19: waiting\n");
    Cond1->Wait(lockTest5);
    printf("CT19: releasing [UNEXPECTED]\n");
    lockTest5->Release();
    printf("CT19: released [UNEXPECTED]\n");
}

void CondThread20(int param) {
    printf("(3)CT20: deleting CV\n");
    Cond1->~Condition();
    printf("CT20: CV deleted [UNEXPECTED]\n");
}

void CondTest9() {
    DEBUG('t', "Entering CondTest9");
    lockTest5 = new Lock("CondTest9_lock");
    Cond1 = new Condition("Cond1");
    Thread *t = new Thread("ninteen");
    t->Fork(CondThread19, 0);
    Thread *t1 = new Thread("twenty");
    t1->Fork(CondThread20, 0);
}


//-----------------------------------//
//Mailbox Tests
//-----------------------------------//

int m = 1;
int m1 = 2;
Mailbox * mailbox1 = NULL;

//mtest1: first receive, then send
void MailboxThread1(int param) {
   printf("MT1:(1)receiving\n");
   mailbox1->Receive(&m);
   printf("MT1:(4)received\n");
}

void MailboxThread2(int param) {
   printf("MT2:(2)sending\n");
   mailbox1->Send(m);
   printf("MT2:(3)sent\n");
}

void MTest1(){
  DEBUG('t', "Entering MTest1");
  
  mailbox1 = new Mailbox("Mailbox1");

  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread1, 0);
  Thread *t1 = new Thread("mthread 2");
  t1->Fork(MailboxThread2, 0);
}

//mtest2: first send then receive
Mailbox * mailbox2 = NULL;
void MailboxThread3(int param){
  printf("MT3:(1)sending\n");
  mailbox2->Send(m);
  printf("MT3:(4)sent\n");
}

void MailboxThread4(int param){
  printf("MT4:(2)receiving\n");
  mailbox2->Receive(&m);
  printf("MT4:(3)received\n");
}

void MTest2(){
  DEBUG('t', "Entering MTest2");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 1");
  t->Fork(MailboxThread3,0);
  t = new Thread("mthread 2");
  t->Fork(MailboxThread4,0);
}

//mtest3: send, send, receive, receive. Seperate threads
void MailboxThread5(int param) {
  printf("MT5:(1)sending first\n");
  mailbox1->Send(m);
  printf("MT5:(7)sent first.\n");
}

void MailboxThread6(int param) {
  printf("MT6:(2)sending second\n");
  mailbox1->Send(m1);
  printf("MT6:(8)sent second\n");
}

void MailboxThread7(int param) {
  printf("MT7:(3)receiving first\n");
  mailbox1->Receive(&m);
  printf("MT7:(4)received second\n");
  
}

void MailboxThread8(int param) {
  printf("MT8:(5)receive second\n");
  mailbox1->Receive(&m1);
  printf("MT8:(6)received second\n");
}

void MTest3(){
  DEBUG('t', "Entering MTest3");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 5");
  t->Fork(MailboxThread5,0);
  t = new Thread("mthread 6");
  t->Fork(MailboxThread6,0);
  t = new Thread("mthread 7");
  t->Fork(MailboxThread7,0);
  t = new Thread("mthread 8");
  t->Fork(MailboxThread8,0);
}

//mtest4: receive, receive, send, send. Seperate threads
void MailboxThread9(int param) {
  printf("MT9:(1)receiving first\n");
  mailbox1->Receive(&m);
  printf("MT9:(7)received first.\n");
}

void MailboxThread10(int param) {
  printf("MT10:(2)receiving second\n");
  mailbox1->Receive(&m1);
  printf("MT10:(8)received second\n");
}

void MailboxThread11(int param) {
  printf("MT11:(3)sending first\n");
  mailbox1->Send(m);
  printf("MT11:(4)sent second\n");
  
}

void MailboxThread12(int param) {
  printf("MT12:(5)sending second\n");
  mailbox1->Send(m1);
  printf("MT12:(6)sent second\n");
}

void MTest4(){
  DEBUG('t', "Entering MTest4");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 9");
  t->Fork(MailboxThread9,0);
  t = new Thread("mthread 10");
  t->Fork(MailboxThread10,0);
  t = new Thread("mthread 11");
  t->Fork(MailboxThread11,0);
  t = new Thread("mthread 12");
  t->Fork(MailboxThread12,0);
}

//MTest5: send
void MailboxThread13(int param) {
  printf("MT13:(1)sending\n");
  mailbox1->Send(m);
  printf("MT13:sent [UNEXPECTED]\n");
}

void MTest5() {
  DEBUG('t', "Entering MTest5");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 13");
  t->Fork(MailboxThread13,0);
}

//MTest6: receive
void MailboxThread14(int param) {
  printf("MT14:(1)receiving\n");
  mailbox1->Receive(&m);
  printf("MT14:received [UNEXPECTED]\n");
}

void MTest6() {
  DEBUG('t', "Entering MTest6");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 14");
  t->Fork(MailboxThread14,0);
}

//MTest7: send, send, receive
void MailboxThread15(int param) {
  printf("MT15:(1)sending\n");
  mailbox1->Send(m);
  printf("MT15:(5)sent\n");
}

void MailboxThread16(int param) {
  printf("MT16:(2)send\n");
  mailbox1->Send(m1);
  printf("MT16:sent [UNEXPECTED]\n");
}

void MailboxThread17(int param) {
  printf("MT17:(3)receiving\n");
  mailbox1->Receive(&m);
  printf("MT17:(4)received\n");
}

void MTest7(){
  DEBUG('t', "Entering MTest7");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 15");
  t->Fork(MailboxThread15,0);
  t = new Thread("mthread 16");
  t->Fork(MailboxThread16,0);
  t = new Thread("mthread 17");
  t->Fork(MailboxThread17,0);
}

//MTest8: receive, receive, send
void MailboxThread18(int param) {
  printf("MT18:(1)receiving\n");
  mailbox1->Receive(&m);
  printf("MT18:(5)received\n");
}

void MailboxThread19(int param) {
  printf("MT19:(2)receiving\n");
  mailbox1->Receive(&m1);
  printf("MT19:received [UNEXPECTED]\n");
}

void MailboxThread20(int param) {
  printf("MT20:(3)sending\n");
  mailbox1->Send(m);
  printf("MT20:(4)sent\n");
}

void MTest8(){
  DEBUG('t', "Entering MTest8");
  
  mailbox1 = new Mailbox("Mailbox1");
  Thread *t = new Thread("mthread 18");
  t->Fork(MailboxThread18,0);
  t = new Thread("mthread 19");
  t->Fork(MailboxThread19,0);
  t = new Thread("mthread 20");
  t->Fork(MailboxThread20,0);
}

//----------------------------------------------------------------------
// Part3Test
//  Invoke a test routine.
//----------------------------------------------------------------------

//test1: the Joiner-Joinee example tester given on write-up
void
Joiner(Thread *joinee)
{
  DEBUG('t', "test0");
  currentThread->Yield();
  DEBUG('t', "test1");
  currentThread->Yield();
  DEBUG('t', "test2");
  printf("Waiting for the Joinee to finish executing.\n");

  currentThread->Yield();
  currentThread->Yield();

  // Note that, in this program, the "joinee" has not finished
  // when the "joiner" calls Join().  You will also need to handle
  // and test for the case when the "joinee" _has_ finished when
  // the "joiner" calls Join().

  joinee->Join();

  currentThread->Yield();
  currentThread->Yield();

  printf("Joinee has finished executing, we can continue.\n");

  currentThread->Yield();
  currentThread->Yield();
}

void
Joinee()
{
  int i;

  for (i = 0; i < 5; i++) {
    printf("Smell the roses.\n");
    currentThread->Yield();
  }

  currentThread->Yield();
  printf("Done smelling the roses!\n");
  currentThread->Yield();
}

void
Part3Test1()
{
  Thread *joiner = new Thread("joiner", 0);  // will not be joined
  Thread *joinee = new Thread("joinee", 1);  // WILL be joined

  // fork off the two threads and let them do their business
  joiner->Fork((VoidFunctionPtr) Joiner, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Joinee, 0);

  // this thread is done and can go on its merry way
  printf("Forked off the joiner and joiner threads.\n");
}


//test2: a thread that will be joined only is destoryed once join has been called on it
Lock* part3Lock = NULL;
Condition* part3Cond = NULL;

void Part3Thread1(Thread *joinee) {
  printf("T1:(1)yield 1\n");
  currentThread->Yield();
  printf("T1:(3)yield 2\n");
  currentThread->Yield();
  printf("T1:(5)yield 3\n");
  currentThread->Yield();
  printf("T1:(6)joining\n");
  if (threadToBeDestroyed == joinee)
    printf("NO!!! joinee is being deleted before join!!!!\n")
  joinee->Join();
  if (threadToBeDestroyed == joinee)
    printf("Great, joinee is being deleted after join\n")
  printf("T1:(7)joined T2\n");
}

void Part3Thread2() {
  printf("T2:(2)yield 1\n");
  currentThread->Yield();
  printf("T2:(4)yielded 1. Ready to finish.\n");
}

void Part3Test2() {
  DEBUG('t', "Entering Part3Test2()");
  Thread *joiner = new Thread("t1",0);
  Thread *joinee = new Thread("t2",1);
  joiner->Fork((VoidFunctionPtr) Part3Thread1, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Part3Thread2, 0);
  printf("Forked off the joiner and joiner threads.\n");
}

//test3: if a parent calls Join on a child and the child is still executing, the parent waits
void Part3Thread3(Thread *joinee) {
  printf("T3:(1)yield 1\n");
  currentThread->Yield();
  printf("T3:(3)joining T4\n");
  if (threadToBeDestroyed == joinee)
    printf("NO!!! joinee is being deleted before join!!!!\n")
  joinee->Join();
  if (threadToBeDestroyed == joinee)
    printf("Great, joinee is being deleted after join\n")
  printf("T3:(8)joined T4\n");
}

void Part3Thread4() {
  printf("T4:(2)yield 1\n");
  currentThread->Yield();
  printf("T4:(4)yield 2\n");
  currentThread->Yield();
  printf("T4:(5)yield 3\n");
  currentThread->Yield();
  printf("T4:(6)yield 4\n");
  currentThread->Yield();  
  printf("T4:(7)yielded 4\n");
}

void Part3Test3() {
  DEBUG('t', "Entering Part3Test3()");
  Thread *joiner = new Thread("t3",0);
  Thread *joinee = new Thread("t4",1);
  joiner->Fork((VoidFunctionPtr) Part3Thread3, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Part3Thread4, 0);
  printf("Forked off the joiner and joiner threads.\n");
}

//test4: if a parent calls Join on a child and the child has finished executing, the parent does not block
void Part3Thread5(Thread *joinee) {
  printf("T5:(1)yield 1\n");
  currentThread->Yield();
  printf("T5:(3)yield 2\n");
  currentThread->Yield();
  printf("T5:(5)yield 3\n");
  currentThread->Yield();
  printf("T5:(6)joining\n");
  if (threadToBeDestroyed == joinee)
    printf("NO!!! joinee is being deleted before join!!!!\n")
  joinee->Join();
  if (threadToBeDestroyed == joinee)
    printf("Great, joinee is being deleted after join\n")
  printf("T5:(7)joined T2\n");
  currentThread->Yield();
  printf("T5:(8)yield 5\n");
  currentThread->Yield();
  printf("T5:(9)yield 6\n");
  currentThread->Yield();
  printf("T5:(10)yielded 6\n");
}

void Part3Thread6() {
  printf("T6:(2)yield 1\n");
  currentThread->Yield();
  printf("T6:(4)yielded 1. Ready to finish.\n");
}

void Part3Test4() {
  DEBUG('t', "Entering Part3Test4()");
  Thread *joiner = new Thread("t5",0);
  Thread *joinee = new Thread("t6",1);
  joiner->Fork((VoidFunctionPtr) Part3Thread5, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Part3Thread6, 0);
  printf("Forked off the joiner and joiner threads.\n");
}

//test5: a thread does not call Join on itself
void Part3Thread7 () {
  printf("T7:(1)Joining myself\n");
  currentThread->Join();
  printf("T7:joined myself [UNEXPECTED]\n");
}

void Part3Test5() {
  DEBUG('t', "Entering Part3Test5()");
  Thread *joinee = new Thread("t7",1);
  joinee->Fork((VoidFunctionPtr) Part3Thread7, 0);
  printf("Forked off the joiner and joiner threads.\n");
}

//test6: Join is only invoked on threads created to be joined
void Part3Thread8(Thread *joinee) {
  printf("T8:(1)joining T2\n")
  joinee->Join();
  printf("T8:joined T2 [UNEXPECTED]\n");
}

void Part3Thread9() {
  printf("T9:yield 1 [UNEXPECTED]\n");
  currentThread->Yield();
  printf("T9:yielded 1. Ready to finish. [UNEXPECTED]\n");
}

void Part3Test6() {
  DEBUG('t', "Entering Part3Test6()");
  Thread *joiner = new Thread("t8",0);
  Thread *joinee = new Thread("t9",0);
  joiner->Fork((VoidFunctionPtr) Part3Thread8, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Part3Thread9, 0);
  printf("Forked off the joiner and joiner threads.\n");
}

//test7: Join is only called on a thread that has forked
void Part3Thread10(Thread *joinee) {
  printf("T8:(1)joining T2\n")
  joinee->Join();
  printf("T8:joined T2 [UNEXPECTED]\n");
}

void Part3Thread10(Thread *joinee) {

}

void Part3Test7() {
  DEBUG('t', "Entering Part3Test7()");
  Thread *joiner = new Thread("t10",0);
  Thread *joinee = new Thread("t11",0);
  joiner->Fork((VoidFunctionPtr) Part3Thread10, (int) joinee);
  printf("Forked off the joiner and joiner threads.\n");
}

//test8: Join is not called more than once on a thread
void Part3Thread11(Thread *joinee) {
  printf("****NOTE: SEGMENTATION FAULT IS ALLOWED IN THIS CASE****\n")
  printf("T11: (1)joining T12 \n");
  joinee->Join();
  printf("T11: (5)joined T12. Joining T12 again\n");
  joinee->Join();
  printf("T11: joined T12 again [UNEXPECTED]\n");
}

void Part3Thread12() {
  printf("T12:(2)yield 1\n");
  currentThread->Yield();
  printf("T12:(3)yield2.\n");
  currentThread->Yield();
  printf("T12:(4)yielded 2. Ready to finish.\n");
}

void Part3Test8() {
  DEBUG('t', "Entering Part3Test8()");
  Thread *joiner = new Thread("t11",0);
  Thread *joinee = new Thread("t12",1);
  joiner->Fork((VoidFunctionPtr) Part3Thread11, (int) joinee);
  joinee->Fork((VoidFunctionPtr) Part3Thread12, 0);
  printf("Forked off the joiner and joiner threads.\n");
}



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
      LockTest5();
  break;
    case 10:
      CondTest1();
  break;
    case 11:
      CondTest2();
  break;
    case 12:
      CondTest3();
  break;
    case 13:
      CondTest4();
  break;
    case 14:
      CondTest5();
  break;
    case 15:
      CondTest6();
  break;
    case 16:
      CondTest7();
  break;
    case 17:
      CondTest8();
  break;
    case 18:
      CondTest9();
  break;
    case 20:
      MTest1();
  break;
    case 21:
      MTest2();
  break;
    case 22:
      MTest3();
  break;
    case 23:
      MTest4();
  break;
    case 24:
      MTest5();
  break;
    case 25:
      MTest6();
  break;
    case 26:
      MTest7();
  break;
    case 27:
      MTest8();
  break;
    case 30:
      Part3Test1();
  break;
    default:
        printf("No test specified.\n");
        break;
    }
}
