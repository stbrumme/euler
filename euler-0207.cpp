// ////////////////////////////////////////////////////////
// # Title
// Integer partition equations
//
// # URL
// https://projecteuler.net/problem=207
// http://euler.stephan-brumme.com/207/
//
// # Problem
// For some positive integers `k`, there exists an integer partition of the form `4^t = 2^t + k`,
// where `4^t`, `2^t`, and `k` are all positive integers and `t` is a real number.
//
// The first two such partitions are `4^1 = 2^1 + 2` and `4^{1.5849625}... = 2^{1.5849625}... + 6`.
//
// Partitions where `t` is also an integer are called perfect.
// For any `m >= 1` let `P(m)` be the proportion of such partitions that are perfect with `k <= m`.
// Thus `P(6) = 1/2`.
//
// In the following table are listed some values of `P(m)`
//
// `P(5) = 1/1`
// `P(10) = 1/2`
// `P(15) = 2/3`
// `P(20) = 1/2`
// `P(25) = 1/2`
// `P(30) = 2/5`
// ...
// `P(180) = 1/4`
// `P(185) = 3/13`
//
// Find the smallest `m` for which `P(m) < 1/12345`
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// The equation can be written as:
// `4^t = 2^t + k`
// `(2^t)^2 = 2^t + k`
//
// And if I replace `x = 2^t`:
// `x^2 = x + k`
//
// My program iterates over all integer values of `x`. Then `x^2` must be an integer and `k` must be an integer, too.
//
// The problem statement already gave away that `x=2` is the smallest valid partition and is perfect, too (which means `t=1` because `2^1=2`).
// My variables ''total'', ''perfect'' and ''x'' are initialized accordingly.
//
// `x = 2^t` is a __perfect__ partition if `t` is an integer. That means that only one bit of the binary representation of `x` is set
// (left side is the power of two, right side how it is written in binary notation):
// `2^1 = 10_2`
// `2^2 = 100_2`
// `2^3 = 1000_2`
// `2^4 = 10000_2`
// `2^5 = 100000_2`
// `2^6 = 1000000_2`
// `2^7 = 10000000_2`
// ...
//
// My bit twiddling code from http://bits.stephan-brumme.com/isPowerOfTwo.html can figure out whether `x` is a power-of-two in just a few CPU cycles.
//
// As soon as the smallest `x` is known such that `dfrac{perfect}{total} < dfrac{1}{12345}`, the formula `x^2 = x + k` is translated to `k = x^2 - x` which produces the correct result.
//
// # Note
// I added a small live test. You can the numerator and denominator for `P(m) = dfrac{numerator}{denominator}`, e.g. `dfrac{1}{1234}` ==> ''1 1234''.

#include <iostream>

// return true, if x is a power of two
bool isPowerOfTwo(unsigned int x)
{
  // see http://bits.stephan-brumme.com/isPowerOfTwo.html
  return (x & (x - 1)) == 0;
}

int main()
{
  // stop when proportion perfect/total is less than numerator/denominator
  unsigned int numerator   = 1;
  unsigned int denominator = 12345;
  std::cin >> numerator >> denominator;

  if (numerator > denominator || numerator == 0) // check input: for live test only
    return 1;

  // count all valid partitions
  unsigned long long total   = 1;
  // count perfect partitions
  unsigned long long perfect = 1;
  // x=2^t
  unsigned long long x = 3;
  // perfect/total > numerator/denominator is the same as
  // perfect*denominator > total*numerator
  while (perfect * denominator > total * numerator)
  {
    // found another perfect partition ?
    if (isPowerOfTwo(x))
      perfect++;

    // keep going ...
    total++;
    x++;
  }

  // find k
  unsigned long long k = x * (x - 1);
  std::cout << k << std::endl;

  return 0;
}
