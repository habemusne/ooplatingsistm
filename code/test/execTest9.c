//execTest9: execute three file each with 500 bytes of code and do not exit
int main(){
  int result = 70;
  char* filename = "../test/codeFile300Byte";
  result = Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  //result += Exec(filename, 0, 0, 0);
  Exit(result);
}
