// ////////////////////////////////////////////////////////
// # Title
// Maximum product of parts
//
// # URL
// https://projecteuler.net/problem=183
// http://euler.stephan-brumme.com/183/
//
// # Problem
// Let `N` be a positive integer and let `N` be split into `k` equal parts, `r = N/k`, so that `N = r + r + ... + r`.
// Let `P` be the product of these parts, `P = r * r * ... * r = r^k`.
//
// For example, if 11 is split into five equal parts, `11 = 2.2 + 2.2 + 2.2 + 2.2 + 2.2`, then `P = 2.2^5 = 51.53632`.
//
// Let `M(N) = P_max` for a given value of `N`.
//
// It turns out that the maximum for `N = 11` is found by splitting eleven into four equal parts which leads to `P_max = (11/4)^4`;
// that is, `M(11) = 14641/256 = 57.19140625`, which is a terminating decimal.
//
// However, for `N = 8` the maximum is achieved by splitting it into three equal parts, so `M(8) = 512/27`, which is a non-terminating decimal.
//
// Let `D(N) = N` if `M(N)` is a non-terminating decimal and `D(N) = -N` if `M(N)` is a terminating decimal.
//
// For example, `sum{D(N)}` for `5 <= N <= 100` is 2438.
//
// Find `sum{D(N)}` for `5 <= N <= 10000`.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// My first approach was to have a simple loop running from k = 1 to n:
// - find the largest `(n/k)^k`
// - figure out whether `(n/k)^k` terminates
//
// When I looked at the values of `k` I saw that `n/k` was always ` approx 2.7`.
// And Wolfram Alpha confirmed my suspicion: http://www.wolframalpha.com/input/?i=max((8%2Fn)%5En)
// `M(8) = max((8/n)^n) = e^{8/e}` at `k = 8/e` where `e = 2.7182818284...`, see https://en.wikipedia.org/wiki/E_(mathematical_constant)
//
// In general: `max((x/n)^n) = e^{n/e}` at `k = n/e`
// Unfortunately, `n/e` isn't an integer and therefore the best `k` is ''k = round(n / 2.718281828)'' (that's enough digits of `e` for this problem)
//
// The number `(n/k)^k` terminates if `n/k` terminates.
// And `n/k` terminates if the greatest common divisor `gcd(n, k)` has only 2 and 5 as prime factors (something I learnt in problem 26).
//
// # Alternative
// As mentioned before, my initial approach computed all `(n/k)^k`.
// But numbers became huge very fast ... well, I didn't need their "true" value: it's sufficient to have their logarithm because if `a < b` then `log(a) < log(b)`, too.
// In the end I converted `(x/n)^n` to `n log(x/n)`.
//
// Using ''round'' can be dangerous if not enough digits of `e` are used. 10 digits are totally fine for that problem, though.
//
// # Hackerrank
// I need to cache intermediate result to be able to handle the huge number of input values.

#include <iostream>
#include <vector>
#include <cmath>

int main()
{
  // five dummy entries
  std::vector<long long> cache(5, 0);

#define ORIGINAL
#ifndef ORIGINAL
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
#endif
  {
    unsigned int limit = 10000;
    std::cin >> limit;

    auto result = cache.back();
    for (unsigned int n = cache.size(); n <= limit; n++)
    {
      unsigned int k = round(n / 2.718281828);

      // terminating only if n/k has only prime factors 2 and 5
      while (k % 5 == 0)
        k /= 5;
      while (k % 2 == 0)
        k /= 2;

      if (n % k == 0)
        result -= n;
      else
        result += n;

      // cache intermediate results, too (needed for Hackerrank only)
      cache.push_back(result);
    }

    // display result
    std::cout << cache[limit] << std::endl;
  }

  return 0;
}
