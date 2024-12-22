#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/*
 * RSA in C.
 *
 * References:
 * 	1. https://en.wikipedia.org/wiki/Primality_test
 * 	2. https://en.wikipedia.org/wiki/RSA_(cryptosystem)
 */

int miller_rabin(int n, int k);
int decompose(int n, int *s, int *d);
int base(int n);
int direct_modular_exponentiation(int a, int d, int n);
int modular_exponentiation(int a, int d, int n);

#define ROUNDS 10

int main() {


	return 0;
}

/*
 * Input:
 * n -> integer to test
 * k -> rounds of testing
 * Return:
 * 1 -> probably a prime
 * 0 -> composite number
 */
int miller_rabin(int n, int k) {
	/*
	 * n -> integer to test
	 * m -> n-1
	 * s -> number of times integer is divisible by 2
	 * d -> remaining odd part
	 */
	int s, d = 0;

	// Edge cases
	if (n <= 1) { return 0; }
	if (n <= 2) { return 1; }
	if (n <= 3) { return 1; }
	if (n % 2 == 0) { return 0; }

	// Decompose to find s and d
	decompose(n, &s, &d);

	for ( int i = 0; i < k; i++ ) {
		int a = base(n); // Choose a random base
		int x = modular_exponentiation(a, d, n);

		// First check
		if (x == 1 || x == n - 1) { continue; }

		// Second check
		int pass = 0;
		for ( int j = 0; j < s-1; j ++ ) {
			x = modular_exponentiation(x, 2, n);
			if (x == n - 1) { pass = 1; break; } // Pass
		}
		if (pass != 1) {return 0;} // Number is a composite
	}
	return 1; // Maybe a prime
}

/*
 * Decomponse an integer to 2^s * d.
 * Used to find values of s and d.
 */
int decompose(int n, int *s, int *d) {
	int m = n - 1;
	*s = 0;
	while ( m % 2 == 0 ) {
		m /= 2;
		(*s)++;
	}
	*d = m;
	return 0;
}

/*
 * Base must be between 2 and n-2
 */
int base(int n) {
	return 2 + rand() % (n-3);
}

/*
 * Direct method of calculating the modular exponentiation.
 * Since we are using low values of d, its okay.
 *
 * Overflows past N=19
 */
int direct_modular_exponentiation(int a, int d, int n) {
	int x = (int)pow(a, d) % n;
	return x;
}

/*
 * Can handle large exponents.
 */
int modular_exponentiation(int a, int d, int n) {
    int result = 1;
    a = a % n;
    while (d > 0) {
        if (d % 2 == 1) {
            result = (result * a) % n;
        }
        a = (a * a) % n;
        d /= 2;
    }
    return result;
}
