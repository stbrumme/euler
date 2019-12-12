// ////////////////////////////////////////////////////////
// # Title
// Prime cube partnership
//
// # URL
// https://projecteuler.net/problem=131
// http://euler.stephan-brumme.com/131/
//
// # Problem
// There are some prime values, `p`, for which there exists a positive integer, `n`, such that the expression `n^3 + n^2 p` is a perfect cube.
//
// For example, when `p = 19`, `8^3 + 8^2 * 19 = 123`.
//
// What is perhaps most surprising is that for each prime with this property the value of `n` is unique, and there are only four such primes below one-hundred.
//
// How many primes below one million have this remarkable property?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// The equation `n^3 + n^2 p = k^3` can be rewritten:
// `n^3 (1 + dfrac{p}{n}) = k^3`
//
// Which becomes:
// `n * sqrt[3]{1 + dfrac{p}{n}} = k`
//
// And:
// `n * sqrt[3]{dfrac{n+p}{n}} = k`
//
// The only way that `n` and `k` are integers is when the cube root is rational:
// `sqrt[3]{dfrac{n+p}{n}} = sqrt[3]{dfrac{a^3}{b^3}} = dfrac{a}{b}`
//
// Then the equation would be:
// `n * dfrac{a}{b} = k`
//
// The new variables `a` and `b` are:
// `a^3 = n+p`
// `b^3 = n`
//
// Solving for `p`:
// `p = a^3 - n = a^3 - b^3`
//
// The binomial expansion `a^3 - b^3 = (a - b)(a^2 + ab + b^2)` tells us:
// `p = (a - b)(a^2 + ab + b^2)`
//
// All values `a`, `b` and `p` must be integers. Moreover, `p` must be a prime. Remember: a prime number can only be factorized into two numbers: 1 and itself.
// Obviously `a - b < a^2 + ab + b^2` (for positive values of `a` and `b`). Then it follows for the factors 1 and `p`:
// `1 = a - b`
// `p = a^2 + ab + b^2`
//
// The first equation is interesting: `a` and `b` are consecutive numbers:
// `a = b + 1`
//
// Which can be inserted in the second equation:
// `p = a^2 + a(a+1) + (a+1)^2`
// `p = a^2 + a^2 + a + a^2 + 2a + 1`
// `p = 3a^2 + 3a + 1`
//
// Whenever `p = 3a^2 + 3a + 1` is prime, then a valid value was found.
// I didn't see it at first, but `(a + 1)^3 - a^3 = 3a^2 + 3a + 1`, too ==> `p` is the difference of two consecutive cubes.
//
// This time I use my Wheel-based primality test (because it's the fastest and I haven't used it in a long time).
// It's part of my [toolbox](../toolbox/#primetest-wheel).
//
// # Hackerrank
// Hackerrank has a huge amount of input values: I have to split my program into two parts. First, I find all solutions up to a ''limit'',
// then I scan those solutions and print the result.
//
// This time all solutions are generated in ascending order. ''std::lower_bound'' finds the largest matching prime below the input very fast (binary search) and returns its position.
// For example: querying 100 returns position 3 because 61 is the largest prime below 100. ''std::distance'' returns 4 because index counting starts at zero.
//
// My primality test is too slow to handle all primes below `25 * 10^12`. About `10^12` is the most I can process in two seconds.
//
// # Note
// I increased ''limit'' to `10^8` for the live test. You only need ''limit = 1000000'' for the original problem.

#include <iostream>
#include <vector>
#include <algorithm>

// wheel-based prime test from my [toolbox](../toolbox/#primetest-wheel)
bool isPrime(unsigned int x)
{
  // prime test for 2, 3 and 5
  if (x % 2 == 0 || x % 3 == 0 || x % 5 == 0)
    return x == 2 || x == 3 || x == 5;

  // wheel with size 30 (=2*3*5):
  // test against 30m+1, 30m+7, 30m+11, 30m+13, 30m+17, 30m+19, 30m+23, 30m+29
  // their deltas/increments are:
  const unsigned char Delta[] = { 6, 4, 2, 4, 2, 4, 6, 2 };
  // start with 7, which is 30*0+7
  unsigned int i   = 7;
  // 7 belongs to the second test group
  unsigned int pos = 1;

  // check numbers up to sqrt(x)
  while (i*i <= x)
  {
    // not prime ?
    if (x % i == 0)
      return false;

    // skip forward to next test divisor
    i += Delta[pos];
    // next delta/increment
    pos = (pos + 1) & 7;
  }

  // passed all tests, must be a prime number
  return x > 1;
}

int main()
{
  // find all matching primes up to this limit
  unsigned int limit = 100000000;

  // store all matching primes
  std::vector<unsigned int> matches;

  // start with a=1
  unsigned int a = 1;
  while (true)
  {
    // (a+1)^3 - a^3 = 3a^2 + 3a + 1
    auto p = 3*a*a + 3*a + 1;
    // too big ?
    if (p >= limit)
      break;

    // found one more prime ?
    if (isPrime(p))
      matches.push_back(p);

    // keep going ...
    a++;
  }

  // process STDIN
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    std::cin >> limit;
    // find highest position located before the limit (matches are sorted, should use binary search)
    auto lower  = std::lower_bound(matches.begin(), matches.end(), limit);
    // count number of primes
    auto result = std::distance(matches.begin(), lower);
    std::cout << result << std::endl;
  }

  return 0;
}
