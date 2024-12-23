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

unsigned long long modular_exponentiation(unsigned long long a,
		unsigned long long d, unsigned long long n);
unsigned long long random_num(unsigned long long min, unsigned long long max);
unsigned long long l_com_mul(unsigned long long a, unsigned long long b);
unsigned long long g_com_div(unsigned long long a, unsigned long long b);
unsigned long long lambda_n(unsigned long long p, unsigned long long q);
unsigned long long pub_exp(unsigned long long lamb_n);
unsigned long long bezout_iden(unsigned long long e, unsigned long long lamb_n);
unsigned long long ext_gcd(unsigned long long a, unsigned long long b,
		long long *x, long long *y);

#define ROUNDS 10

int main() {
	srand(time(0));
	unsigned long long min = 1ULL << 1;
	unsigned long long max = (1ULL << 32) - 1;

	unsigned long long p;
	do {
		p = random_num(min, max);
		if (p % 2 == 0) { p++; }
	} while (miller_rabin(p, ROUNDS) != 1);
	//printf("p -> %llu\n", p);

	unsigned long long q;
	do {
		q = random_num(min, max);
		if (q % 2 == 0) { q++; }
	} while (miller_rabin(q, ROUNDS) != 1);
	//printf("q -> %llu\n", q);

	unsigned long long lamb_n = lambda_n(p, q);
	//printf("lamb_n -> %llu\n", lamb_n);
	
	unsigned long long e = pub_exp(lamb_n);
 	//printf("Public exponent e -> %llu\n", e);

	printf("Public key = (%llu, %llu)\n", p * q, e);

	unsigned long long d = bezout_iden(e, lamb_n);
 	printf("Private key = %llu\n", d);

	return 0;
}

/*
 * Solve for d and k in Bezouts identity:
 * 						d * e - k * lamb(n) = 1
 */
unsigned long long bezout_iden(unsigned long long e,
		unsigned long long lamb_n) {
	long long x, y;
	unsigned long long gcd = ext_gcd(e, lamb_n, &x, &y);

	// e and lamb_n are not coprimes
	if (gcd != 1) {
		return 0;
	}

	return (x % lamb_n + lamb_n) % lamb_n;
}

/*
 * Extended Euclidean algorithm:
 * 						d * e + k * lamb_n(n) = gcd(e, lamb(n))
 */
unsigned long long ext_gcd(unsigned long long a, unsigned long long b,
		long long *x, long long *y) {
	if (b == 0) {
		*x = 1;
		*y = 0;
		return a;
	}

	long long x1, y1;
	unsigned long long gcd = ext_gcd(b, a % b, &x1, &y1);

	*x = y1;
	*y = x1 - (a / b) * y1;

	return gcd;
}

/*
 * Find the public exponent. Most common value is 65537.
 */
unsigned long long pub_exp(unsigned long long lamb_n) {
	// Common working e value
	unsigned long long e = 65537;
	if (g_com_div(e, lamb_n)) {
		return e;
	}
	// Fallback if 65537 does not work
	for (e = 3; e < lamb_n; e += 2) {
		if (g_com_div(e, lamb_n)) {
			return e;
		}
	}
	// uh oh.
	 return 0;
}

/*
 * Compute carmichaels totient function using:
 * 						lambda(n) = lcm(p - 1, q - 1)
 */
unsigned long long lambda_n(unsigned long long p, unsigned long long q) {
	return l_com_mul(p - 1, q - 1);
}

/*
 * Find the least common multiple using:
 * 						lcm(a, b) = |a*b| / gcd(a,b)
 */
unsigned long long l_com_mul(unsigned long long a, unsigned long long b) {
	return (a / g_com_div(a, b)) * b;
}

/*
 * Find the greatest common divisor using Euclidean algorithm:
 * 						gcd(a,b) = gcd(b, a mod b)
 */
unsigned long long g_com_div(unsigned long long a, unsigned long long b) {
	while (b != 0) {
		unsigned long long temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

/*
 * Generate a large number in a given range. If we generate a very large number,
 * it is possible a prime will not be found for quite a while so this could do
 * with parallelization and better mathematics involved. For now, the range is
 * quite small.
 *
 * Input:
 * min -> minimum number
 * max -> maximum number
 *
 * Output:
 * Potentially large number, used to find a prime for P and Q in RSA
 *
 */
unsigned long long random_num(unsigned long long min, unsigned long long max) {
	unsigned long long range = max - min + 1;
	unsigned long long high = (unsigned long long)rand() << 32;
	unsigned long long low  = (unsigned long long)rand();
	unsigned long long result = (high | low) % range + min;

	return result;
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
		unsigned long long x = modular_exponentiation(a, d, n);

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
unsigned long long modular_exponentiation(unsigned long long a,
										  unsigned long long d,
										  unsigned long long n) {
    unsigned long long result = 1;
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
