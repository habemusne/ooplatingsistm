//execTest8: execute three 500 byte files (so that none of two will
//  fit into memory if simutaneously running), but each file exits
//  after execution
//EXPECTED: normal exit
int main(){
  int result = 0;
  result = Exec("../test/exitFile300Byte", 0, 0, 0);
  result = Exec("../test/exitFile300Byte", 0, 0, 0);
  result = Exec("../test/exitFile300Byte", 0, 0, 0);
  Exit(result);
}
