
//execTest2: pass in a filename that does not exist
int main(){
  int result = 20;
  result = Exec("../test/notexist", 0, 0, 0);
  Exit(result);
}
