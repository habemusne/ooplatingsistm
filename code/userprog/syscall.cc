
void ProcessStart(AddrSpace* space) {
  space->InitRegisters();   // set the initial register values
  space->RestoreState();    // load page table register

  machine->Run();     // jump to the user progam
  ASSERT(FALSE);      // machine->Run never returns;

}

void Exit(int status) {
 
   printf("The sum is: %d\n", status);
   currentThread->Finish();
}

SpaceId Exec(char *name, int argc, char **argv, int opt) {

  if (name == NULL){
    printf("syscall.cc: Passing in null directory name\n");
    return;
  }

  

  OpenFile *executable = fileSystem->Open(filename);
  AddrSpace *space;

  if (executable == NULL) {
    printf("Unable to open file %s\n", filename);
    return;
  }

  space = new AddrSpace(executable);
  space->Initialize();


  Thread* newThread = new Thread("thread", 0);
  newThread->space = space;
  newThread->Fork(ProcessStart, (int) space);


  delete executable;      // close file

}
