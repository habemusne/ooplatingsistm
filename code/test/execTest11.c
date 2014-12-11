//execTest11: 10 processes
int main(){
  int result = 0;
  char* filename = "../test/exitFile300Byte";
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  result += Exec(filename, 0, 0, 0);
  Exit(result);
}
