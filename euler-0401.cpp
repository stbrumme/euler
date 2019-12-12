// ////////////////////////////////////////////////////////
// # Title
// Sum of squares of divisors
//
// # URL
// https://projecteuler.net/problem=401
// http://euler.stephan-brumme.com/401/
//
// # Problem
// The divisors of 6 are 1,2,3 and 6.
// The sum of the squares of these numbers is 1+4+9+36=50.
//
// Let `sigma2(n)` represent the sum of the squares of the divisors of `n`. Thus `sigma2(6)=50`.
// Let `SIGMA2` represent the summatory function of `sigma2`, that is `SIGMA2(n)=sum{sigma2(i)}` for `i=1` to `n`.
// The first 6 values of `SIGMA2` are: 1,6,16,37,63 and 113.
//
// Find `SIGMA2(10^15) mod 10^9`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Any number `i` has `\lfloor frac{n}{i} \rfloor` multiples (integer division).
// When calculating `SIGMA2(6)`:
// - `1` has `\lfloor frac{6}{1} \rfloor = 6` multiples, add `6 * 1^2 = 6 * 1 = 6`
// - `2` has `\lfloor frac{6}{2} \rfloor = 3` multiples, add `3 * 2^2 = 3 * 4 = 24`
// - `3` has `\lfloor frac{6}{3} \rfloor = 2` multiples, add `2 * 3^2 = 2 * 9 = 18`
// - `4` has `\lfloor frac{6}{4} \rfloor = 1` multiples, add `1 * 4^2 = 1 * 16 = 16`
// - `5` has `\lfloor frac{6}{5} \rfloor = 1` multiples, add `1 * 5^2 = 1 * 25 = 25`
// - `6` has `\lfloor frac{6}{6} \rfloor = 1` multiples, add `1 * 6^2 = 1 * 36 = 36`
// `SIGMA2(6) = 6 + 24 + 18 + 16 + 25 + 36 = 113`
//
// Unfortunately this requires `n` iterations, which is no problem for `n = 6` but next to impossible for `n = 15`.
// Looking closer at the example reveals that half of the divisors appears only once (`4`, `5` and `6`).
// In general, a few number appear very often and the majority just a few times.
// Because `i` appears `\lfloor frac{n}{i} \rfloor` times, `i <= \lfloor frac{n}{i} \rfloor` if `i <= sqrt{n}`.
// If `i > sqrt{n}` then several squares appear the same time. As mentioned before, there are three squares that appear once.
//
// My program applies the basic algorithm for all numbers up to `sqrt{n}`, which needs `sqrt{10^15} approx 3.2 * 10^7` iterations.
// Then it determines which squares appear once, twice, etc. going backwards from `10^15` to `sqrt{n}`.
// This can be quite efficient because the sum of a series of squares is (see https://en.wikipedia.org/wiki/Square_pyramidal_number ):
// `P(n) = sum_{k=1...n}{k^2} = dfrac{n (n + 1) (2n + 1)}{6}`
//
// The largest square pyramidal number that appears exactly `m` times is `\lfloor frac{n}{m} \rfloor`.
// To obtain the sum of all square that appear `m` times I calculate `P(\lfloor frac{n}{m} \rfloor) - P(\lfloor frac{n}{m+1} \rfloor)`.
//
// ''pyramidal(n)'' returns the n-th square pyramidal number (`mod 10^9`). This function is rather tricky because I hit the limits of C++'s 64 bit integers.
// Luckily, G++ has routines for 128 bit integers to work around this issue - but therefore this code doesn't compile with Visual C++.
//
// The code is scattered with `mod` statement. Maybe a few are redundant.
//
// # Note
// Apparently you can implement ''pyramidal'' without ''__int128''.

#include <iostream>
#include <cmath>

// return sum of 1^2 + 2^2 + 3^2 + 4^2 + ... + n^2 mod modulo
// note: needs GCC's 128 bit integers (not available on Visual C++)
unsigned int pyramidal(unsigned long long n, unsigned int modulo)
{
  // inspired by https://stackoverflow.com/questions/39826483/square-pyramidal-number-of-int32-modulo-some-m-using-only-int64-intermediates
  unsigned __int128 nn = n;
  // solve n * (n + 1) * (2n + 1) / 6
  // either nn or nn+1 is even => their product must be divisible by 2
  auto x = nn * (nn + 1) / 2;
  auto y = 2 * n + 1;
  // now equation becomes x * y / 3 (mod modulo)

  // is x a multiple of 3 ?
  auto div3 = x / 3;
  if (div3 * 3 == x)
    return ((div3 % modulo) * y) % modulo;

  // no, y must be a multiple of 3
  return (x % modulo) * (y / 3) % modulo;
}

// count all divisors
unsigned int solve(unsigned long long n, unsigned int modulo)
{
  unsigned long long sum = 0;

  // for each number i there is a second divisor n/i (integer division)
  // for i < sqrt(n) those second divisors are "unique"
  // for i > sqrt(n) several i share the same second divisor n/i
  unsigned int threshold = sqrt(n);

  // look at small divisors
  for (unsigned long long i = 1; i <= threshold; i++)
  {
    // each divisor-th number is a multiple of divisor
    auto count  = (n / i) % modulo;
    // square (reduce with modulo)
    auto square = (i * i) % modulo;

    // add
    sum += (count * square) % modulo;
  }

  // apply the same logic in reverse order:
  // for each divisor i there is a divisor j = n / i
  for (unsigned int j = 1; j <= n / (threshold + 1); j++)
  {
    auto from = n / j;
    auto to   = n / (j + 1);

    unsigned long long sumOfConsecutiveSquares = 0;
    // note: from > to, therefore decrement instead of increment
    /*for (unsigned long long k = from; k > to; k--)
    {
      auto square = (k * k) % modulo;
      sumOfConsecutiveSquares += square;
      sumOfConsecutiveSquares %= modulo;
    }*/

    // same as the for-loop above
    auto squareSumFrom = pyramidal(from, modulo); // sum of squares <= from
    auto squareSumTo   = pyramidal(to,   modulo); // sum of squares <= to
    // add modulo to prevent negative numbers
    if (squareSumFrom < squareSumTo)
      squareSumFrom += modulo;
    sumOfConsecutiveSquares = squareSumFrom - squareSumTo;

    // those squares can be found "j" times
    sum += (j * sumOfConsecutiveSquares) % modulo;
  }

  // and again, take care of modulo
  return sum % modulo;
}

int main()
{
  // 10^9
  const auto Modulo = 1000000000;
  // 10^15
  auto n = 1000000000000000ULL;
  std::cin >> n;
  std::cout << solve(n, Modulo) << std::endl;
  return 0;
}
