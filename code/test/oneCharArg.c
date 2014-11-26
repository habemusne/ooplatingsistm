int main(int argc, char *argv[]){
  if (argc == 1){
    if (argv[0][0] == 'a'){
      Exit(11);
    } else {
      Exit(-1);
    }
  } else {
    Exit(-11);
  }
}
