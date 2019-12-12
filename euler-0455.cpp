// ////////////////////////////////////////////////////////
// # Title
// Powers With Trailing Digits
//
// # URL
// https://projecteuler.net/problem=455
// http://euler.stephan-brumme.com/455/
//
// # Problem
// Let `f(n)` be the largest positive integer `x` less than `10^9` such that the last 9 digits of `n^x` form the number `x`
// (including leading zeros), or zero if no such integer exists.
//
// For example:
//
// `f(4) = 411728896` (`4^411728896 = ...411728896`)
// `f(10) = 0`
// `f(157) = 743757` (`157^743757 = ...000743757`)
// `sum{f(n)}`, `2 <= n <= 10^3 = 442530011399`
//
// Find `sum{f(n)}`, `2 <= n <= 10^6`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// The ''bruteForce()'' algorithm is super-slow and actually longer than my final solution.
// And I found my final solution by chance: I used the last iteration's result as exponent instead of my loop variable.
// `x_{i+1} = n^{x_i} mod 10^9`
//
// It turns out that there is "fix point" - if you repeat the procedure long enough then you will observe
// `x_{j+1} = x_j`
//
// The only exception are powers of 10 where `x_j` will become zero.
//
// I was astonished to find that the initial value `x_0` doesn't really matter.
// The number of iterations varies a little bit and seems to be quite low when `x_0 = n`.

#include <iostream>
#include <vector>

// ---------- 32 bit powmod from my toolbox ----------

// return (base^exponent) % modulo for 32-bit values, no need for mulmod
unsigned int powmod(unsigned int base, unsigned int exponent, unsigned int modulo)
{
  unsigned int result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = (result * (unsigned long long)base) % modulo;

    // even exponent ? a^b = (a*a)^(b/2)
    base = (base * (unsigned long long)base) % modulo;
    exponent >>= 1;
  }
  return result;
}

// ---------- problem specific code ----------

// try every possible exponent ... takes many minutes per number
unsigned int bruteForce(unsigned int n, unsigned int modulo)
{
  unsigned int result = modulo;
  while (result > 1)
  {
    result--;
    if (powmod(n, result, modulo) == result)
      break;
    if (result % 1000000 == 0)
      std::cout << n << " " << result << std::endl;
  }
  return result;
}

// find maximum exponent e such that n^e = e mod modulo
unsigned int search(unsigned int n, unsigned int modulo)
{
  unsigned int exponent = n;
  do
  {
    auto next = powmod(n, exponent, modulo);
    // stuck in a loop => we're finished
    if (next == 0 || next == exponent)
      return next;

    // keep going ...
    exponent = next;
  } while (true);
}

int main()
{
  unsigned int limit = 1000000;
  std::cin >> limit;

  const unsigned int Modulo = 1000000000;

  //std::cout << bruteForce(4, Modulo) << std::endl;

  unsigned long long sum = 0;
  for (unsigned int i = 2; i <= limit; i++)
    sum += search(i, Modulo);

  std::cout << sum << std::endl;
  return 0;
}
