#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"

class SynchConsole
{
   public:
      SynchConsole();
      ~SynchConsole();

      void SynchPutChar(char ch);
      char SynchGetChar();

      void SynchPutString(char *c, int n);   //write the string to the terminal
      int SynchGetString(char *c, int n);   //Poll the console input string

   private:
      Semaphore *mutex;
      Console* console;
};

extern SynchConsole *synchConsole;
#endif
