#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"

class SynchConsole
{
   public:
      SynchConsole(char *readFile, char *writeFile);
      ~SynchConsole();

      void SynchPutChar(char ch);
      char SynchGetChar();

      void SynchPutString(char *c);   //write the string to the terminal
      int SynchGetString(char *c, int n);   //Poll the console input string

   private:
      Semaphore *mutex;
      Console* console;
};

#endif
