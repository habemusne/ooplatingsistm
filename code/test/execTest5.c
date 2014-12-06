
//execTest5: passing in an executable that has too large code size
int main(){
  int result = 50;
  char* filename = "../test/largeCodeFile";
  result = Exec(filename, 0, 0, 0);
  Exit(result);
}
