// ////////////////////////////////////////////////////////
// # Title
// Fibonacci golden nuggets
//
// # URL
// https://projecteuler.net/problem=137
// http://euler.stephan-brumme.com/137/
//
// # Problem
// Consider the infinite polynomial series `A_F(x) = xF_1 + x^2 F_2 + x^3 F_3 + ...`, where `F_k` is the `k`th term in the Fibonacci sequence: 1, 1, 2, 3, 5, 8, ... ;
// that is, `F_k = F_{k-1} + F_{k-2}`, `F_1 = 1` and `F_2 = 1`.
//
// For this problem we shall be interested in values of `x` for which `A_F(x)` is a positive integer.
// Surprisingly `A_F(1/2) = (1/2) * 1 + (1/2)^2 * 1 + (1/2)^3 * 2 + (1/2)^4 * 3 + (1/2)^5 * 5 + ...`
//  `= 1/2 + 1/4 + 2/8 + 3/16 + 5/32 + ...`
//  `= 2`
//
// The corresponding values of `x` for the first five natural numbers are shown below.
//
// ||         9        ||     5    ||
// ||!  `x`            ++ `A_F(x)` ||
// || `2-1`            ++     1    ||
// || `1/2`            ++     2    ||
// || `(sqrt{13}-2)/3` ++     3    ||
// || `(sqrt{89}-5)/8` ++     4    ||
// || `(sqrt{34}-3)/5` ++     5    ||
//
// We shall call `A_F(x)` a golden nugget if `x` is rational, because they become increasingly rarer; for example, the 10th golden nugget is 74049690.
//
// Find the 15th golden nugget.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Admittedly, I had no idea. And then I just searched the OEIS for 74049690 (the 10th nugget).
// Fortunately, the first hit https://oeis.org/A081018 contained a list of numbers where 74049690 was at the 10th position. Jackpot ...
//
// According to that website, the n-th golden nugget is the product of `F(2n) * F(2n+1)`.
// All I have to solve is `F(2 * 15) * F(2 * 15 + 1) = F(30) * F(31)` - I can do that instantly !
//
// # Hackerrank
// This problem turned from "no idea" to "extremely easy" to "quite hard" when I saw Hackerrank's modifications:
// I have to process up to `10^5` input values in 2 seconds where some input values can be as large as `10^18`.
// (And the result has to be printed modulo `10^9+7`.)
//
// Searching the web for fast Fibonacci algorithms I came across the "matrix form" (see https://en.wikipedia.org/wiki/Fibonacci_number ):
// `\left( \begin{matrix} F_{n+2} \\ F_{n+1} \end{matrix} \right) = \left( \begin{matrix} 1 & 1 \\ 1 & 0 \end{matrix} \right) \left( \begin{matrix} F_{n+1} \\ F_{n} \end{matrix} \right)`
//
// This looks to be even slower than the old-fashioned way of adding ... but there is a twist:
// `\left( \begin{matrix} F_{n+1} \\ F_{n} \end{matrix} \right) = \left( \begin{matrix} 1 & 1 \\ 1 & 0 \end{matrix} \right)^n \left( \begin{matrix} F_1 \\ F_0 \end{matrix} \right)`
//
// Raising the matrix to the n-th power can be done pretty quickly: fast exponentiation needs about `log_2(10^18) approx 60` iterations to calculate the final matrix for `F(10^18)`.
// The concept is the same as in ''powmod'' but instead of multiplying single numbers I have to perform a matrix multiplication.
//
// The most beautiful property of the matrix form is that I get the successor of the desired Fibonacci for free.
//
// Using these tricks my program solves 100% at Hackerrank.

#include <iostream>

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
  // (a * b) % modulo = (a % modulo) * (b % modulo) % modulo
  a %= modulo;
  b %= modulo;

  // fast path
  if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    return (a * b) % modulo;

  // we might encounter overflows (slow path)
  // the number of loops depends on b, therefore try to minimize b
  if (b > a)
    std::swap(a, b);

  // bitwise multiplication
  unsigned long long result = 0;
  while (a > 0 && b > 0)
  {
    // b is odd ? a*b = a + a*(b-1)
    if (b & 1)
    {
      result += a;
      if (result >= modulo)
        result -= modulo;
      // skip b-- because the bit-shift at the end will remove the lowest bit anyway
    }

    // b is even ? a*b = (2*a)*(b/2)
    a <<= 1;
    if (a >= modulo)
      a -= modulo;

    // next bit
    b >>= 1;
  }

  return result;
}

// Fibonacci matrix algorithm (taken from my solution of problem 304)
// return (Fibonacci(2n) * Fibonacci(2n+1)) % modulo
unsigned long long nugget(unsigned long long n, unsigned long long modulo)
{
  // n-th nugget is based on Fibonacci(2n) and Fibonacci(2n+1)
  n *= 2;

  // fast exponentiation: same idea as powmod from my toolbox

  // matrix values from https://en.wikipedia.org/wiki/Fibonacci_number#Matrix_form
  unsigned long long fibo  [2][2]= { { 1, 1 },
                                     { 1, 0 } };
  // initially identity matrix
  unsigned long long result[2][2]= { { 1, 0 },   // { { F(n+1), F(n)   },
                                     { 0, 1 } }; //   { F(n),   F(n-1) } }

  while (n > 0)
  {
    // fast exponentation:
    // odd exponent ? a^n = a*a^(n-1)
    if (n & 1)
    {
      // compute new values, store them in temporaries
      auto t00 = mulmod(result[0][0], fibo[0][0], modulo) + mulmod(result[0][1], fibo[1][0], modulo);
      auto t01 = mulmod(result[0][0], fibo[0][1], modulo) + mulmod(result[0][1], fibo[1][1], modulo);
      auto t10 = mulmod(result[1][0], fibo[0][0], modulo) + mulmod(result[1][1], fibo[1][0], modulo);
      auto t11 = mulmod(result[1][0], fibo[0][1], modulo) + mulmod(result[1][1], fibo[1][1], modulo);

      if (t00 >= modulo) t00 -= modulo;
      if (t01 >= modulo) t01 -= modulo;
      if (t10 >= modulo) t10 -= modulo;
      if (t11 >= modulo) t11 -= modulo;

      // copy back to matrix
      result[0][0] = t00; result[0][1] = t01;
      result[1][0] = t10; result[1][1] = t11;
    }

    // even exponent ? a^n = (a*a)^(n/2)

    // compute new values, store them in temporaries
    auto t00 = mulmod(fibo[0][0], fibo[0][0], modulo) + mulmod(fibo[0][1], fibo[1][0], modulo);
    auto t01 = mulmod(fibo[0][0], fibo[0][1], modulo) + mulmod(fibo[0][1], fibo[1][1], modulo);
    auto t10 = mulmod(fibo[1][0], fibo[0][0], modulo) + mulmod(fibo[1][1], fibo[1][0], modulo);
    auto t11 = mulmod(fibo[1][0], fibo[0][1], modulo) + mulmod(fibo[1][1], fibo[1][1], modulo);

    if (t00 >= modulo) t00 -= modulo;
    if (t01 >= modulo) t01 -= modulo;
    if (t10 >= modulo) t10 -= modulo;
    if (t11 >= modulo) t11 -= modulo;

    // copy back to matrix
    fibo[0][0] = t00; fibo[0][1] = t01;
    fibo[1][0] = t10; fibo[1][1] = t11;

    n >>= 1;
  }

  // F(2n + 1) * F(2n)
  return (result[0][0] * result[0][1]) % modulo;
}

int main()
{
#define ORIGINAL
#ifdef ORIGINAL
  // a number large enough such that it basically disables modular arithmetic for the original problem
  const unsigned long long Modulo = 10000000000000ULL;
#else
  // 10^9 + 7
  const unsigned long long Modulo =     1000000007;
#endif

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long n;
    std::cin >> n;
    std::cout << nugget(n, Modulo) << std::endl;
  }
  return 0;
}
