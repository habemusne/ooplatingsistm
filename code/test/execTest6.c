
//execTest6: passing in a file that has too large initData on heap
int main(){
  int result = 60;
  char* filename = "../test/largeDataFile";
  result = Exec(filename, 0, 0, 0);
  Exit(result);
}
