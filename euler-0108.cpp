// ////////////////////////////////////////////////////////
// # Title
// Diophantine reciprocals I
//
// # URL
// https://projecteuler.net/problem=108
// http://euler.stephan-brumme.com/108/
//
// # Problem
// In the following equation `x`, `y`, and `n` are positive integers.
//
// `dfrac{1}{x} + dfrac{1}{y} = dfrac{1}{n}`
//
// For `n = 4` there are exactly three distinct solutions:
//
// `dfrac{1}{5} + dfrac{1}{20} = dfrac{1}{4}`
//
// `dfrac{1}{6} + dfrac{1}{12} = dfrac{1}{4}`
//
// `dfrac{1}{8} + dfrac{1}{8}  = dfrac{1}{4}`
//
// What is the least value of n for which the number of distinct solutions exceeds one-thousand?
//
// NOTE: This problem is an easier version of Problem 110; it is strongly advised that you solve this one first.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// It's safe to assume `x <= y`. Moreover, `x > n` because `frac{1}{x} < frac{1}{n}`. That means `y > n`, too.
//
// There must be some `a` and `b` such that `x = n + a` and `y = n + b`.
// The original equation becomes:
//
// `dfrac{1}{n + a} + dfrac{1}{n + b} = dfrac{1}{n}`
//
// Multiply both sides by `n`:
//
// `dfrac{n}{n + a} + dfrac{n}{n + b} = dfrac{n}{n} = 1`
//
// Multiply by `n+a`:
//
// `dfrac{n(n+a)}{n + a} + dfrac{n(n+a)}{n + b} = n + a`
//
// `n + dfrac{n(n+a)}{n + b} = n + a`
//
// And the same for `n+b`:
//
// `n(n+b) + dfrac{n(n+a)(n+b)}{n + b} = (n + a)(n + b)`
//
// `n(n+b) + n(n+a) = (n + a)(n + b)`
//
// Simplify:
// `n^2 + nb + n^2 + na = n^2 + na + nb + ab`
// `n^2 = ab`
//
// In the example where `n=4` you can compute `x = n + a` ==> `5 = 4 + a` ==> `a = 1` and because of `y = n + b` ==> `b = 16`.
// Indeed, `ab = 1 * 16 = 16 = 4^2 = n^2`.
// (And for the other two solutions: `ab = (6-4) * (12-4) = 16 = n^2` and `ab = (8-4) * (8-4) = 16 = n^2`).
//
// What does it mean ? Well, finding all `ab = n^2` produces all solutions.
// The number of solutions is the number of divisors of `n^2`.
//
// My first attempt was based on brute force and is at the end of the code (it's not used anymore).
// A smarter approach is to perform a prime factorization.
//
// A high number of divisors means that prime factors must be small.
// To speed up the program I abort when a prime factor > 100 is left. This is kind of cheating ... and roughly 100x faster.
//
// # Hackerrank
// I didn't notice that prime factorization of `n^2` can be reduced to a slightly modified prime factorization of `n`.
// Thanks to http://www.mathblog.dk/project-euler-108-diophantine-equation/ where I found that trick (see ''numSquareDivisors'')
// The code becomes much faster but I kept my old code ''numDivisors'' for the original problem.
//
// However, I still time-out on two thirds of the test cases (my old code timed out in 80% of all cases).

#include <iostream>

//#define ORIGINAL

// count divisors
unsigned long long numDivisors(unsigned long long n)
{
  unsigned int result = 1;
  auto reduce = n;
  // trial division by all prime numbers
  // => I didn't precompute a sieve, therefore divide by 2 and all odd numbers
  for (unsigned long long divisor = 2; divisor <= reduce; divisor++)
  {
    // 2 is the only even prime number
    if (divisor % 2 == 0 && divisor > 2)
      divisor++;

    if (divisor > 100) // WARNING: unsafe speed optimization !
      break;           // returns correct values for original problem but fails for some Hackerrank test cases

    unsigned int exponent = 0;
    while (reduce % divisor == 0)
    {
      exponent++;
      reduce /= divisor;
    }

    result *= exponent + 1;
  }

  return result;
}

// count divisors of n^2, note: parameter is n, not n^2 (this is different from my old code in numDivisors)
unsigned long long numSquareDivisors(unsigned long long n)
{
  unsigned int result = 1;
  auto reduce = n;
  // trial division by all prime numbers
  // => I didn't precompute a sieve, therefore divide by 2 and all odd numbers
  for (unsigned long long divisor = 2; divisor <= reduce; divisor++)
  {
    // 2 is the only even prime number
    if (divisor % 2 == 0 && divisor > 2)
      divisor++;

    unsigned int exponent = 0;
    while (reduce % divisor == 0)
    {
      exponent++;
      reduce /= divisor;
    }

    result *= 2*exponent + 1; // changed vs. my code: times 2
  }

  return result;
}

int main()
{
#ifdef ORIGINAL

  unsigned long long n = 1;
  unsigned long long threshold = 1000;

  while (true)
  {
    auto divisors = numDivisors(n * n);

    // a and b are interchangeable therefore only half of the solutions are "unique"
    auto half = (divisors + 1) / 2; // plus 1 because n^2 is obviously a square number
    if (half >= threshold)
    {
      std::cout << n << std::endl;
      break;
    }

    // check next square number
    n++;
  }

#else

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // find the number of solutions
    unsigned long long n;
    std::cin >> n;

    auto divisors = numSquareDivisors(n);

    // a and b are interchangeable therefore only half of the solutions are "unique"
    auto half = (divisors + 1) / 2; // plus 1 because n^2 is obviously a square number
    std::cout << half << std::endl;
  }

#endif

  return 0;
}

// my initial brute force solution
int bruteForce(unsigned int threshold)
{
  unsigned long long n = 4;
  while (true)
  {
    unsigned int solutions = 0;

    // try all values between n+1 ... 2n
    for (unsigned long long x = n + 1; x <= 2*n; x++)
    {
      // the same as 1/x + 1/y = 1/n
      auto y = n*x / (x - n);
      // integer arithmetic might produce a wrong result, re-compute the formula backwards
      if (y * (x - n) != n*x)
        continue;

      // exhausted ?
      if (y < x)
        break;

      // valid solution
      solutions++;
    }

    if (solutions > threshold)
      break;

    n++;
  }

  std::cout << n << std::endl;
  return 0;
}
