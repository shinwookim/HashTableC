#include <math.h>
#include "prime.h"

/*
 * Return whether x is prime or not
 *
 * Returns:
 *   1  - prime
 *   0  - not prime
 *   -1 - undefined (i.e. x < 2)
 */

/**
 * Returns whether x is a prime number.
 *
 * @return 1 if x is prime, 0 if x is not prime, and -1 if x is undefined
 */
int is_prime(const int x)
{
    if (x < 2)
        return -1;
    if (x < 4)
        return 1;
    if ((x % 2) == 0)
        return 0;
    for (int i = 3; i <= floor(sqrt((double)x)); i += 2)
    {
        if ((x % i) == 0)
            return 0;
    }
    return 1;
}

/**
 * Returns the smallest prime number that is greater or equal to x
 */
int next_prime(int x)
{
    while (is_prime(x) != 1)
        x++;
    return x;
}