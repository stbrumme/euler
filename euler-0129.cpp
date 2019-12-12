// ////////////////////////////////////////////////////////
// # Title
// Repunit divisibility
//
// # URL
// https://projecteuler.net/problem=129
// http://euler.stephan-brumme.com/129/
//
// # Problem
// A number consisting entirely of ones is called a repunit. We shall define `R(k)` to be a repunit of length `k`; for example, `R(6) = 111111`.
//
// Given that `n` is a positive integer and `GCD(n, 10) = 1`, it can be shown that there always exists a value, `k`, for which `R(k)` is divisible by n,
// and let `A(n)` be the least such value of `k`; for example, `A(7) = 6` and `A(41) = 5`.
//
// The least value of `n` for which `A(n)` first exceeds ten is 17.
//
// Find the least value of `n` for which `A(n)` first exceeds one-million.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// Only if `x` is a divisor of `y` then `y mod x == 0`.
// My code generates all repunits `R(k)` until `R(k) mod x == 0`:
// `R(k+1) = R(k) * 10 + 1`
//
// Unfortunately, `R(20)` is already too big for a 64 bit integer.
// But I don't actually need to work with `R(k)`, I can use its `mod x`, too:
// `R(k) mod x = R_{mod x}(k) mod x`
// `R_{mod x}(k+1) = (R_{mod x}(k) * 10 + 1) mod x`
//
// `x` fits easily in a 32 bit integer, therefore the "modulo repunit" fits in there, too.
//
// Initially I started a brute-force search with ''i = 1'' which took a few minutes. However, it seems that always `A(i) <= i`.
//
// # Hackerrank
// Apparently there is a much smarter way to solve this - that I have no idea about. All but the first test case time out.

#define ORIGINAL

#include <iostream>

// return minimum k where R(k) is divisible by x
unsigned long long getMinK(unsigned long long x)
{
  // same as gcd(x, 10) = 1
  if (x % 5 == 0 || x % 2 == 0)
    return 0;

  // "number of ones"
  unsigned long long result  = 1;
  // current repunit mod divisor
  unsigned long long repunit = 1;
  // no remainder ? that repunit can be divided by divisor
  while (repunit != 0)
  {
    // next repunit
    repunit *= 10;
    repunit++;
    // keep it mod divisor
    repunit %= x;

    result++;
  }

  return result;
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // I observed that getMinK(i) <= i
    // I lack the mathematic skills to prove (or disprove) it
    // at least it holds up to the result when running a brute-force search beginning at 1
    unsigned long long limit = 1000000;
    std::cin >> limit;
    unsigned long long i = limit;

#ifdef ORIGINAL
    while (getMinK(i) <= limit)
      i++;
    std::cout << i << std::endl;
#else
    std::cout << getMinK(i) << std::endl;
#endif
  }
  return 0;
}
