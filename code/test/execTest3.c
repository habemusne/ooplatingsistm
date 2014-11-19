
//execTest3: pass in a filename that is too large
int execTest3(){
  int result = 30;
  char* filename = "ThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilenameThisisafilewithfreakinglongfilename.c";
  result = Exec(filename, 0, 0, 0);
  Exit(result);
}
