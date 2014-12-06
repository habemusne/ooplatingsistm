
//execTest2: pass in a filename that does not exist
int main(){
  int result = 20;
  char* filename = "../test/notexist";
  result = Exec(filename, 0, 0, 0);
  Exit(result);
}
