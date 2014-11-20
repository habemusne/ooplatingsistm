
//execTest3: pass in a filename that is too large
int main(){
  int result = 30;
  char* filename = "ThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilename.c";
  result = Exec(filename, 0, 0, 0);
  Exit(result);
}
