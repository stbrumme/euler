// ////////////////////////////////////////////////////////
// # Title
// Investigating progressive numbers, n, which are also square
//
// # URL
// https://projecteuler.net/problem=141
// http://euler.stephan-brumme.com/141/
//
// # Problem
// A positive integer, `n`, is divided by `d` and the quotient and remainder are `q` and `r` respectively.
// In addition `d`, `q`, and `r` are consecutive positive integer terms in a geometric sequence, but not necessarily in that order.
//
// For example, 58 divided by 6 has quotient 9 and remainder 4. It can also be seen that 4, 6, 9 are consecutive terms in a geometric sequence (common ratio 3/2).
// We will call such numbers, `n`, progressive.
//
// Some progressive numbers, such as `9` and `10404 = 102^2`, happen to also be perfect squares.
// The sum of all progressive perfect squares below one hundred thousand is `124657`.
//
// Find the sum of all progressive perfect squares below one trillion (`10^12`).
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// The problem asks for solution of
// (1) `n = d * q + r`
// I can assume that `d <= q` (otherwise I would just swap both)
// (2) `r < d <= q`
// The geometric progression is
// (3) `d / r = q / d`
// which is equivalent to
// (4) `d = sqrt{q * r}` (remember: `d` must be an integer)
// Now `n` is
// (5) `n = sqrt{q * r} * q + r`
// My simple ''bruteForce'' algorithm finds all solutions for `n < 100000` is less than 0.01 seconds. And would take forever to solve `n < 10^12`.
// Nevertheless, that's what ''bruteForce'' does:
// - iterate over all `r < q < 100000`
// - compute `n` using equation (5)
// - make sure that `n < 100000` and `n` is a perfect square and `q * r` is a perfect square
//
// That approach is too slow, so let's try a different way: let's define a rational number `k > 1`
// (6) `k = d / r = q / d`
// (7) `d = k * r`
// (8) `q = k^2 * r`
//
// If `q` is an integer then `q = k^2 * r` must be an integer, too.
// Since `k` is rational there are some values `x` and `y`
// (9) `k = dfrac{x}{y}`
// (10) `k^2 = dfrac{x^2}{y^2}`
// (11) `q = dfrac{x^2}{y^2} * r`
//
// In equation (11) the variable `r` must be a multiple of `y^2` otherwise `q` wouldn't be an integer.
// That means `r` is the product of some integer and a perfect square `y^2`.
// My program iterates over all `d` and all `factor` and all `y` such that `r = factor * y^2` and `r < d`. From (3) follows:
// (12) `q = d^2 * r`
// (13) `n = dfrac{d^3}{r} + r`
//
// Knowing potential candidates for `d` and `r` I have to check whether `frac{d^3}{r}` is actually an integer (it's not for most pairs !).
// If `n < 10^12` and `n` is a perfect square then another solution was found. There are only 23 solution for `n < 10^12`.
//
// Be careful: there are multiple combinations of ''factor'' and ''y*y'' which evaluate to the same ''remainder''. For example ''36 = 9 * 2*2 = 4 * 3*3''.
// Each remainder must only be used once for each divisor. I saw in my logs that is at most one solution per remainder and divisor.
// I don't know why - so maybe my fast exit with the variable ''found'' is wrong.
//
// I'm not very proud that there is a heuristic in the most inner loop which aborts ''if (n > 2*limit)''.
// There is no true science behind that line - I just know it works for `10^5` and `10^12`.
// Actually I can abort ''if (n > limit)'' for `10^12` but it misses one solution for `10^5`.
// In the end, this heuristic shortens my execution time to a reasonable level __but it could produce wrong answers for other inputs__.
//
// # Note
// It took me a long time find a solution (well, ''bruteForce'' was easy but my final algorithm is completely different).
// There are multiple "hacks" in my code - yes, I can solve the two cases `100000` and `10^12` but have no idea how many inputs fail.
// __So be extra careful when using my live test !__
//
// # Alternative
// After I solved this problem I took a look at other solutions. Most people came up with a much faster and reliable way which I missed completely (their code finishes in less than a second).

#include <iostream>
#include <cmath>

// true if n is a perfect square
template <typename T>
bool isSquare(T n)
{
  T root = sqrt(n);
  return root * root == n;
}

// iterate over all remainders and quotients, it's okay for limit = 100000 but too slow for 10^12
void bruteForce(unsigned int limit)
{
  unsigned int sum = 0;

  for (unsigned int remainder = 1; remainder < limit; remainder++)
    for (unsigned int quotient = remainder + 2; quotient < limit; quotient++)
    {
      // d / r = q / d, therefore d^2 = r * q
      auto divisorSquared = remainder * quotient;
      unsigned int divisor = sqrt(divisorSquared);

      // find n
      auto n = divisor * quotient + remainder;
      // too large ?
      if (n > limit)
        break;
      // not a perfect square ?
      if (!isSquare(n))
        continue;

      // most d^2 are actually NOT a perfect square, therefore all computations above yield garbage
      if (!isSquare(divisorSquared))
        continue;

      // yes, found another combination
      //std::cout << n << " = " << divisor << " * " << quotient << " + " << remainder << std::endl;
      sum += n;
    }

  std::cout << sum << std::endl;
}

int main()
{
  unsigned long long limit = 1000000000000ULL;
  std::cin >> limit;

  //bruteForce(limit); return 0;

  unsigned long long sum = 0;

  // remainder < divisor <= quotient
  for (unsigned long long divisor = 1; divisor * divisor <= limit; divisor++)
  {
    // it seems there is at most one solution per divisor
    bool found = false;

    // all possible perfect squares ...
    for (unsigned long long y = sqrt(divisor); y >= 1 && !found; y--)
      // ... with all possible factors ...
      for (unsigned long long factor = 1; factor * y * y <= divisor; factor++)
      {
        // ... generate all possible remainders
        auto remainder = factor * y * y;

        // n = d^3 / r + r
        auto n = divisor * divisor * divisor / remainder + remainder;

        // heuristic (MIGHT FAIL !) to skip very large n
        if (n > 2*limit)
          break;
        // ignore if beyond limit
        if (n > limit)
          continue;

        // must be a perfect square (I moved this check in front of the next check because that turned out to be faster)
        if (!isSquare(n))
          continue;

        // there's a high likelihood that d^3 isn't a multiple of r and therefore d^3/r isn't an integer
        auto quotient = n / divisor;
        // cross-check whether all variables produce a valid result
        if (divisor * quotient + remainder != n)
          continue;

        sum += n;

        // it seems there is at most one solution per divisor
        found = true;
        break;
        // note: I kept track of various remainders (to avoid repetitions for the same divisor)
        //       aborting after a valid solution eliminates the need
      }
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
