//execTest10: execute three 500 byte files
int main(){
  int result = 0;
  char* filename = "../test/exitFile300Byte";
  result = Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  Exit(result);
}
