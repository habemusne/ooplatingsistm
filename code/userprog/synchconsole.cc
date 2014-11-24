#include "synchconsole.h"

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

SynchConsole *synchConsole = new SynchConsole(NULL, NULL);


SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
   readAvail = new Semaphore("readAvail", 0);
   writeDone = new Semaphore("writeAvail", 0);
   mutex = new Semaphore("mutex", 0);
   printf("I believe the bug is here!!!!!!!!!!!!!!!!!\n");
   console = new Console(readFile, writeFile, (VoidFunctionPtr)ReadAvail, (VoidFunctionPtr)WriteDone, 0);
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
   console->PutChar(ch);
   writeDone->P();
   mutex->V();
}

char SynchConsole::SynchGetChar()
{
   char ch;
   mutex->P();
   readAvail->P();
   ch = console->GetChar();
   mutex->V();

   return ch;
}

void SynchConsole::SynchPutString(char *c, int n)
{
   for(int i = 0; i < n; i++)
   {
      SynchPutChar(c[i]);
   }
}

int SynchConsole::SynchGetString(char *c, int n)
{
   char ch;
   int i;
   for(i = 0; i < n; i++)
   {
      ch = SynchGetChar();
      c[i] = ch;

      if(ch == '\0')
    	 break;
   }

   return i;
}
