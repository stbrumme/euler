// ////////////////////////////////////////////////////////
// # Title
// Totient permutation
//
// # URL
// https://projecteuler.net/problem=70
// http://euler.stephan-brumme.com/70/
//
// # Problem
// Euler's Totient function, `phi(n)` (sometimes called the phi function), is used to determine the number of positive numbers
// less than or equal to n which are relatively prime to n.
// For example, as 1, 2, 4, 5, 7, and 8, are all less than nine and relatively prime to nine, `phi(9) = 6`.
//
// The number 1 is considered to be relatively prime to every positive number, so `phi(1) = 1`.
//
// Interestingly, `phi(87109) = 79180`, and it can be seen that 87109 is a permutation of 79180.
//
// Find the value of n, `1 < n < 10^7`, for which `phi(n)` is a permutation of `n` and the ratio `dfrac{n}{phi(n)}` produces a minimum.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// The function ''phi(x)'' computes `phi(x)`:
// - divide ''x'' by all prime numbers
// - count how many distinct prime numbers were divisors of ''x''
//
// The original formula for the totient is (see https://en.wikipedia.org/wiki/Euler%27s_totient_function ):
// `result = x * (1 - 1/prime1) * (1 - 1/prime2) * (1 - 1/prime3) * ...`
// e.g. if x == 10
// `result = 10 * (1 - 1/2) * (1 - 1/5) = 4`
//
// When initializing ''result = x'' then the multiplication can be reduced to a subtraction:
// whenever we find a prime factor ''p'', then ''result -= result/p''.
// For `phi(10)` we have the prime factors `2` and `5`
// `10 * (1 - 1/2) * (1 - 1/5)`
// `= (10 * (1 - 1/2)) * (1 - 1/5)`
// `= (10 - 10/2) * (1 - 1/5)` ==> ''result -= result/2''
// `= 5 * (1 - 1/5)`
// `= 5 - 5/5` ==> ''result -= result/5''
// `= 4`
//
// In this iterative algorithm, ''result'' is becoming smaller and smaller after each step.
// If it becomes obvious that the result is too small (''minQuotient < x/result = result * minQuotient < x''), then ''phi'' aborts.
// This small optimization gives a 2.5x performance boost.
//
// My ''fingerprint'' function was used in several Project Euler problems before, e.g. problem 49, problem 52, problem 62, ...
//
// # Alternative
// I bet you can severely speed up the program by incorporating some observations:
// - minimizing `n/phi(n)` means maximizing `phi(n)`
// - `phi(n)` is maximized for prime numbers
// - if i is not a prime, then `phi(n)` is still big if it contains just a few prime factors and those are close to `sqrt{n}`

#include <iostream>
#include <vector>

// precompute all primes less than sqrt(10^7)
std::vector<unsigned int> primes;

// return phi(x) if x/phi(x) <= minQuotient (else the result is undefined but > minQuotient)
unsigned int phi(unsigned int x, double minQuotient)
{
  // totient function can be computed by finding all prime factors p
  // and subtracting them from x
  auto result  = x;
  auto reduced = x;
  for (auto p : primes)
  {
    // prime factors have to be p <= sqrt
    if (p*p > reduced)
      break;

    // not a prime factor ...
    if (reduced % p != 0)
      continue;

    // prime factors may occur multiple times, remove them all
    do
    {
      reduced /= p;
    } while (reduced % p == 0);

    // but subtract from result only once
    result -= result / p;

    // abort, this number can't be relevant because the quotient is already too high
    if (result * minQuotient < x)
      return result; // number is garbage but always >= its correct value
  }

  // prime number (result is still equal to x because we couldn't find any prime factors)
  if (result == x)
    return x - 1;
  // (actually this case would be properly handled by the next if-clause, too)

  // we only checked prime factors <= sqrt(x)
  // there might exist one (!) prime factor > sqrt(x)
  // e.g. 3 is a prime factor of 6, and 3 > sqrt(6)
  if (reduced > 1)
    return result - result / reduced;
  else
    return result;
}

// count digits, two numbers have the same fingerprint if they are permutations of each other
unsigned long long fingerprint(unsigned int x)
{
  unsigned long long result = 0;
  while (x > 0)
  {
    auto digit = x % 10;
    x /= 10;

    unsigned long long shift = 1;
    for (unsigned int i = 0; i < digit; i++)
      shift *= 10;

    result += shift;
  }
  return result;
}

int main()
{
  unsigned int last;
  std::cin >> last;

  // step 1: the usual prime sieve
  primes.push_back(2);
  for (unsigned int i = 3; i*i < last; i += 2)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto p : primes)
    {
      // next prime is too large to be a divisor ?
      if (p*p > i)
        break;

      // divisible ? => not prime
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    // yes, we have a prime number
    if (isPrime)
      primes.push_back(i);
  }

  // step 2: analyze all phi(n)
  unsigned int bestNumber  = 2;
  double       minQuotient = 999999;
  for (unsigned int n = 3; n < last; n++)
  {
    auto phi_n = phi(n, minQuotient);
    double quotient = n / double(phi_n);
    // already have a better quotient ?
    if (minQuotient <= quotient)
      continue;

    // check if phi(n) is a permutation of n
    if (fingerprint(phi_n) == fingerprint(n))
    {
      minQuotient = quotient;
      bestNumber  = n;
    }
  }

  // print result
  std::cout << bestNumber << std::endl;
  return 0;
}
