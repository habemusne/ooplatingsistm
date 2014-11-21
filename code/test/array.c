/*
 * array.c
 *
 * A simple program that has code and data and fits into memory.
 */

int Exec(char *, int, char **, int);
int Exit(int);

int array[500];

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
    fill_array(500);
    verify_array(500);
    sum = add_array(500);
    verify_array(500);

    Exit(sum);
}
