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

      void SynchPutString(int c, int n);   //write the string to the terminal
      int SynchGetString(int c, int n);   //Poll the console input string

   private:
      Semaphore *mutex;
      Console* console;
};

extern SynchConsole *synchConsole;
#endif
