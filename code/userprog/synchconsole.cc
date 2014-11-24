#include "synchconsole.h"

synchConsole = new SynchConsole();

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail()
{ 
   readAvail->V(); 
}

static void WriteDone() 
{
   writeDone->V();
}


SynchConsole::SynchConsole()
{
   readAvail = new Semaphore("readAvail", 1);
   writeDone = new Semaphore("writeAvail", 1);
   mutex = new Semaphore("mutex", 1);
   console = new Console(readFile, writeFile, (VoidFunctionPtr) ReadAvail, (VoidFunctionPtr) WriteDone, 0);
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
   ch = console->GetChar();
   readAvail->P();
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
