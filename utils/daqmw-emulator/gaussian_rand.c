#include <sys/time.h>

#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* generate random number between 0 and 1 */
double rand_0_1(void)
{
	int i;
	double x;
	i = rand();
	x = (double) i / (double) RAND_MAX;
	return x;
}
	
double gaussian_rand(void)
{
	double u1, u2; /* uniformly distributed random numbers */
	double w;      /* variance, then a weight */
	double g1;     /* gaussian-distrubuted numbers */
	
	do {
		u1 = 2.0*rand_0_1() - 1.0;
		u2 = 2.0*rand_0_1() - 1.0;
		w  = u1*u1 + u2*u2;
	} while (w >= 1.0 || w == 0.0);
	w = sqrt( -2*log(w) / w);
	g1 = u2*w;
	return g1;
}

/* sample main *********************************************************
int main(int argc, char *argv[])
{
	int i, n_data;
	int sdev;
	int mean;
	double x;
	struct timeval start, end, diff;

	if (argc != 4) {
		errx(1, "Usage: ./gaussian_rand <mean> <sdev> <n_data>");
	}

	mean   = strtol(argv[1], NULL, 0);
	sdev   = strtol(argv[2], NULL, 0);
	n_data = strtol(argv[3], NULL, 0);
	
	srand(time(NULL));
	gettimeofday(&start, NULL);
	for (i = 0; i < n_data; i++) {
		x = gaussian_rand()*sdev + mean;
		printf("%10.4f\n", x);
	}
	gettimeofday(&end, NULL);
	timersub(&end, &start, &diff);
	fprintf(stderr, "%d events in %ld.%06ld\n", n_data, diff.tv_sec, diff.tv_usec);

	return 0;
}
************************************************************************ */
