#include "synchconsole.h"
#include "system.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg)
{ 
   readAvail->V(); 
}
static void WriteDone(int arg) 
{
   writeDone->V();
}

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
   readAvail = new Semaphore("readAvail", 0);
   writeDone = new Semaphore("writeAvail", 0);
   mutex = new Semaphore("mutex", 1);
   console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
   delete console;
   delete mutex;
   delete readAvail;
   delete writeDone;
}

void SynchConsole::SynchPutChar(char ch)
{
   mutex->P();
  // printf("*************mark1");
   console->PutChar(ch);  //echo the char!
  // printf("*************mark2");
   writeDone->P();  //wait for write to finish
   mutex->V();
}

char SynchConsole::SynchGetChar()
{
   char ch;
   mutex->P();

   readAvail->P();  //wait for character to arrive
   ch = console->GetChar();

   mutex->V();

   return ch;
}

void SynchConsole::SynchPutString(int c, int n)
{
  // printf("*********************mark123");


   char ch = 9;
   for(int i = 0; i < n; i++)
   {
     // printf("*********************mark3");
      ch = *currentThread->space->vir_to_phys(c + i);

      SynchPutChar(ch);
   }
}

int SynchConsole::SynchGetString(int c, int n)
{
   char ch;
   int i;
   for(i = 0; i < n; i++)
   {
      ch = SynchGetChar();
      //c[i] = ch;
      *currentThread->space->vir_to_phys(c + i) = ch;

      if(ch == '\0')
    	 break;
   }

   return i;
}
