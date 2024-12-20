#include <stdio.h>
#include <stdlib.h>


/*
 * Simple program to generate random doubles between upper and lower bounds,
 * then write them to a file.
 *
 * Written numbers are smaller than expected for sake of simplicity. Large
 * numbers are generated but written to the file as smaller numbers.
 */

int main() {
	double lower = 1e-200;
	double upper = 1e200;

	// Open file
	FILE *file = fopen("numbers.txt", "w");
	if (file == NULL) {
		perror("Cant open file");
		return 1;
	}

	// Rand numbers and write to file
	int N = 10000;
	double temp;
	srand(time(NULL));
	for (int i = 0; i < N; i++){ 
		temp = lower + ((double)rand() / RAND_MAX) * (upper - lower);
		fprintf(file, "%d\n", temp);
	}

	// flush and close file
	fflush(file);
	fclose(file);

	return 0;
}
