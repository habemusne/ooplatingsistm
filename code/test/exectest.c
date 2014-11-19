
//execTest1: pass in a NULL filename
int execTest1(){
  int result = 20;
  result = Exec(NULL, 0, 0, 0);
  return (result);
}

//execTest2: pass in a filename that does not exist
int execTest2(){
  int result = 30;
  result = Exec("../test/notexist", 0, 0, 0);
  return (result);
}

//execTest3: pass in a filename that is larger than PageSize (128 bytes)
int execTest3(){
  int result = 40;
  char[] filename = new char[129];
  for (int i = 0; i < 129; ++i){
    filename[i] = 'a';
  }
  result = Exec("../test/notexist", 0, 0, 0);
  return (result);
}

//execTest4: pass in a argv that is larger than PageSize
int execTest4(){
  int result = 50;
  char[] argv = new char[129];
  for (int i = 0; i < 129; ++i){
    argv[i] = 'a';
  }
  result = Exec("../test/halt.c", 0, &argv, 0);
  return (result);
}

int execTest5(){
  int result = 10;
  result = Exec("../test/exittest", 0, 0, 0);
  return (result);
}

int main(){
  printf("%d\n", execTest1());
  printf("%d\n", execTest2());
  printf("%d\n", execTest3());
  printf("%d\n", execTest4());
  printf("%d\n", execTest5());
  Exit(0);
}
