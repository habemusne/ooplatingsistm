
//execTest6: passing in a file that has too large initData on heap
int main(){
  int result = 0;
  result = Exec("largeDataFile", 0, 0, 0);
  Exit(result);
}
