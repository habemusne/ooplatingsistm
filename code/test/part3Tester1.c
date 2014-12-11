/*
 * part3Tester1.c
 *
 * A test program that references all of the pages in memory.
 * This test program generate good locality because all the elements in 
 * a large global int array are accessed.
 *
 *  If running successfully, the exit return value would be 
 *  49999 * 50000 / 2 =  1,249,975,000
 */

int Exec(char *, int, char **, int);
int Exit(int);

int array[50000];

void fill_array(int n)
{
   int i;
   for(i=0; i<n; i++)
   {
      array[i] = i;
   }
}
void
verify_array(int n)
{
    int i;

    for (i = 0; i < n; i++) {
  if (array[i] != i) {
      Exit(-1);
  }
    }
}

int
add_array(int n)
{
    int i, sum;

    sum = 0;
    for (i = 0; i < n; i++) {
  sum += array[i];
    }
    return sum;
}

int
main ()
{
    int sum;
    fill_array(50000);
    verify_array(50000);
    sum = add_array(50000);
    verify_array(50000);

    Exit(sum);
}