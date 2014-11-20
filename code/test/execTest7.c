//execTest7: execute three files, each with 500 bytes of code
int main(){
  int result = 70;
  result = Exec("../test/codeFile300Byte", 0, 0, 0);
  result = Exec("../test/codeFile300Byte", 0, 0, 0);
  result = Exec("../test/codeFile300Byte", 0, 0, 0);
  Exit(result);
}
