// ////////////////////////////////////////////////////////
// # Title
// Squarefree Numbers
//
// # URL
// https://projecteuler.net/problem=193
// http://euler.stephan-brumme.com/193/
//
// # Problem
// A positive integer `n` is called squarefree, if no square of a prime divides `n`, thus 1, 2, 3, 5, 6, 7, 10, 11 are squarefree, but not 4, 8, 9, 12.
//
// How many squarefree numbers are there below `2^50`?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// My program counts all non-squarefree numbers and then displays `2^50 - notSquarefree`.
//
// Any number can be written as a product of its prime factors:
// `x = {p_1}^{e_1} * {p_2}^{e_2} * ... * {p_n}^{e_n}`
// If `x` is squarefree, then all its exponents are `e_1 = e_2 = ... = e_n = 1`.
//
// A non-squarefree number, let's call it "squary", has at least one exponent `e_i > 1`.
// The largest prime factor to be considered for finding squaries must be `{p_i}^2 < 2^50` or `p_i < sqrt{2^50}`.
//
// The first step in my program is to count the number of distinct prime factors for all numbers below `sqrt{2^50}` (these numbers are my "base numbers").
// The idea is that all multiples `k * b^2` of these "base numbers" `b` aren't squarefree.
// However, all relevant "base number" must be squarefree themselves. That's why I track all "squary base numbers" in my ''ignore'' array.
//
// The number of multiples `k * b^2` of a "squary base number" ''i'' is ''numMultiples = limit / (b * b)''.
//
// There is one more problem - some numbers are counted multiple times.
// For example `192 = 2^2 * 7^2` is a multiple of 2, 7 and 14 (=2*7).
// A pretty sweet workaround solves this problem:
// - if the number of prime factors of a base number is odd, then add the number of multiples
// - if the number of prime factors of a base number is even, then the number of multiples was already added twice: once for each pair of prime factors
// ==> in that case, subtract it

#include <iostream>
#include <vector>
#include <cmath>

int main()
{
  unsigned long long limit = 1ULL << 50;
  std::cin >> limit;

  // largest prime factor that may appear as a square
  unsigned long long root  = (unsigned long long)sqrt(limit);

  // count number of distinct prime factors
  std::vector<unsigned char> numPrimeFactors(root, 0);
  // does any prime factor occurs multiple times ?
  std::vector<bool> ignore(root, false);
  for (unsigned long long prime = 2; prime < root; prime++)
  {
    // skip if not a prime number
    if (numPrimeFactors[prime] != 0)
      continue;

    // add the prime factor to all multiples
    for (auto j = prime; j < root; j += prime)
      numPrimeFactors[j]++;

    // all multiples have at least one prime factor multiples times, mark as invalid
    auto square = prime * prime;
    for (auto j = square; j < root; j += square)
      ignore[j] = true;
  }

  // count all numbers that are not squarefree
  unsigned long long notSquarefree = 0;
  for (unsigned long long base = 2; base < root; base++)
  {
    // at least one prime factor occurs multiple times ?
    if (ignore[base])
      continue;

    // all multiples are not squarefree
    auto square = base * base;
    auto numMultiples = limit / square;

    // if the number of prime factors is odd, then these multiples are new
    if (numPrimeFactors[base] % 2 == 1)
      notSquarefree += numMultiples;
    else // else: when even number of prime factors, then we have seen these numbers before
      notSquarefree -= numMultiples;
  }

  // display result
  auto result = limit - notSquarefree;
  std::cout << result << std::endl;
  return 0;
}
