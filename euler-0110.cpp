// ////////////////////////////////////////////////////////
// # Title
// Diophantine reciprocals II
//
// # URL
// https://projecteuler.net/problem=110
// http://euler.stephan-brumme.com/110/
//
// # Problem
// In the following equation `x`, `y`, and `n` are positive integers.
//
// `dfrac{1}{x} + dfrac{1}{y} = dfrac{1}{n}`
//
// It can be verified that when `n = 1260` there are 113 distinct solutions and this is the least value of `n`
// for which the total number of distinct solutions exceeds one hundred.
//
// What is the least value of `n` for which the number of distinct solutions exceeds four million?
//
// NOTE: This problem is a much more difficult version of Problem 108 and as it is well beyond the limitations of a brute force approach it requires a clever implementation.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// As mentioned in my solution for problem 108, a prime factorization can find the number of solutions.
// This time I "invert" the algorithm: I generate prime factorizations in ascending order.
// If `e_0`, `e_1`, `e_2`, ... are the exponents of the first primes 2, 3, 5, ... then:
// `value = 2^{e_0} * 3^{e_1} * 5^{e_2}` ...
// The container ''exponents'' stores these exponents, while ''prime'' obviously holds the prime numbers.
// Empirically I found that the first 12 prime numbers (2 ... 37) are sufficient for the original problem.
//
// This gives `((2e_0 + 1)(2e_1 + 1)(2e_2 + 1) ... + 1)` factorizations. However, `x` and `y` are interchangeable, therefore I must divide the result by 2 as well.
//
// A nice property of ''std::map'' is that its elements are always sorted by their keys.
// Therefore ''todo.begin()'' always refers to the smallest unprocessed number.
// If it has too few prime factorizations then I increment its components and re-insert them into ''todo''.
//
// # Note
// I noticed that the exponents of larger prime numbers are at most 1 (when processing numbers <= 4000000).
// My code runs about 3x faster and needs less memory when I avoid incrementing `e_4`, `e_5`, ...
//
// # Hackerrank
// Hackerrank has a broader input ranges which requires a few more prime numbers.
// Even more, the result may be too big for 64 bit integer. That's why I opt for ''long double'' instead.
// The failed test case is probably due to exceeding the precision of ''long double''.

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

int main()
{
  unsigned long long limit = 4000000;
  std::cin >> limit;

#define ORIGINAL
#ifdef ORIGINAL
  // 12 primes are enough for the original problem, I added a few more for the Hackerrank version
  const unsigned int NumPrimes = 12;
  // a short list of primes
  unsigned int primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };
#else
  const unsigned int NumPrimes = 18;
  // a short list of primes
  unsigned int primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61 };
#endif

  // use 1 as a "seed" value (all its primes' exponents are zero)
  typedef std::vector<unsigned char> Exponents;
  std::map<long double, Exponents> todo = { { 1, Exponents(NumPrimes, 0) } };

  while (true)
  {
    // pick the smallest value and its exponents
    auto current   = todo.begin();
    auto value     = current->first;
    auto exponents = current->second;

    // remove from todo list
    todo.erase(current);

    // find number of factorizations
    unsigned long long uniqueFactors = 1;
    for (auto x : exponents)
      uniqueFactors *= 2 * x + 1;
    // a*b = b*a => divide number of factorizations by 2
    uniqueFactors++;
    uniqueFactors /= 2;

    // stop ?
    if (uniqueFactors >= limit)
    {
      std::cout << std::fixed << std::setprecision(0) << value << std::endl;
      break;
    }

    // increment each primes' exponent by 1 and append new value to todo list
    for (size_t i = 0; i < exponents.size(); i++)
    {
#ifdef ORIGINAL
      // speed optimization: exponents of larger primes can be at most 1
      if (exponents[i] == 1 && i > 3) // 3 was found by trial-n-error
        break;
#endif

      exponents[i]++;
      value *= primes[i];

      todo[value] = exponents;
      // note: std::map prunes duplicates automatically
    }
  }

  return 0;
}
