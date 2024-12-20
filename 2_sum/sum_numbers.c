#include <stdio.h>
#include <stdlib.h>

/*
 * Read signed integers from file and compute the running sum.
 *
 * Would be better if the values were extremely large numbers but this is okay
 * for now.
 */

int main() {
	FILE *file = fopen("numbers.txt", "r");
	if (file == NULL) {
		perror("Cant open file");
		return 1;
	}

	double sum = 0;
	char buffer[256];
	while (fgets(buffer, sizeof(buffer), file)) {
		//printf("buffer: %s", buffer);
		double temp;
		if (sscanf(buffer, "%lf", &temp) != 1) {
			printf("Failed to parse integer on line: %s", buffer);
		}
		sum = sum + temp;
	}

	fflush(file);
	fclose(file);

	printf("Total sum: %lf\n", sum);

	return 0;
}
