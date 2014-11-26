int main(){
  int result = 10;
  char* arg = "a";
  result = Exec("../test/oneCharArg", 1, arg, 0);
  Exit(result);
}
