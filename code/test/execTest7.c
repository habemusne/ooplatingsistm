//execTest7: execute one file with 500 bytes of code and does not exit
int main(){
  int result = 70;
  char* filename = "../test/codeFile300Byte";
  result = Exec(filename, 0, 0, 0);
  Exit(result);
}
