/*
 * part3Random.c
 *
 * A test program that reference the random pages.
 * Poor locality.
 */


int array[1000][1000];
unsigned int seed;


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

/* a very poor random number generator */
unsigned int irand(int l, int h)
{
  unsigned int a = 1588635695, m = 429496U, q = 2, r = 1117695901;
  unsigned int val;

  seed = a*(seed % q) - r*(seed / q);
  val = (seed / m) % (h - l) + l;
 
  return val;
}

int main ()
{
    fill_array(1000);
    seed = 93186752;
    int i = 0;
    int sum = 0;
    while(i++ < 1000) {
    	unsigned int random1 = irand(0, 999);
      unsigned int random2 = irand(0, 999);

	    sum += array[random1][random2];
    }


    Exit(sum);
}

