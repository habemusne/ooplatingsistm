/*
 * part3Tester2.c
 *
 * A test program that references different pages in memory.
 * This test program generate random locality because the elements in 
 * a large global 2D array are placed in random pages.
 *
 *  If running successfully, the exit return value would be 
 *  99 * 100 / 2 = 4950
 */

int Exec(char *, int, char **, int);
int Exit(int);

int array[100][100];

void fill_array(int n)
{
   int i;
   int j;
   for(i=0; i<n; i++)
      for(j=0; j<n; j++)
      {
        array[i][j] = i;
      }
}
void
verify_array(int n)
{
    int i;
    int j;
   for(i=0; i<n; i++)
      for(j=0; j<n; j++) {
  if (array[i][j] != i) {
      Exit(-1);
  }
    }
}

int
add_array(int n)
{
    int i, sum, j;

    sum = 0;
    for (i = 0; i < n; i++)
       for(j = 0; j < n; j++) {
       if(i == j)
           sum += array[i][j];
    }
    return sum;
}

int
main ()
{
    int sum;
    fill_array(100);
    verify_array(100);
    sum = add_array(100);
    verify_array(100);

    Exit(sum);
}
