// ////////////////////////////////////////////////////////
// # Title
// Linear Combinations of Semiprimes
//
// # URL
// https://projecteuler.net/problem=278
// http://euler.stephan-brumme.com/278/
//
// # Problem
// Given the values of integers `1 < a_1 < a_2 < ... < a_n`, consider the linear combination
// `q_1 a_1 + q_2 a_2 + ... + q_n a_n = b`, using only integer values `q_k >= 0`.
//
// Note that for a given set of `a_k`, it may be that not all values of `b` are possible.
// For instance, if `a_1 = 5` and `a_2 = 7`, there are no `q_1 >= 0` and `q_2 >= 0` such that `b` could be
// 1, 2, 3, 4, 6, 8, 9, 11, 13, 16, 18 or 23.
// In fact, 23 is the largest impossible value of `b` for `a_1 = 5` and `a_2 = 7`.
// We therefore call `f(5, 7) = 23`.
// Similarly, it can be shown that `f(6, 10, 15) = 29` and `f(14, 22, 77) = 195`.
//
// Find `sum{f(pq,pr,qr)}`, where `p`, `q` and `r` are prime numbers and `p < q < r < 5000`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Assume I have two numbers `x` and `y` where `gcd(x,y)=1`.
// The value `m = xy - x - y` can't be represented with some coefficients `m = px + qy` because:
// `xy - x - y = px + qy`
// `xy = px + qy + x + y`
// `xy = (p+1)x + (q+1)y`
//
// `xy` is a multiple of `x` and `(p+1)x` is a multiple of `x`, hence `(q+1)y` should be a multiple of `x`, too.
// `xy` is a multiple of `y` and `(q+1)y` is a multiple of `y`, hence `(p+1)x` should be a multiple of `y`, too.
// But `gcd(x,y)=1` so `y` can't be a multiple of `x` and therefore `q+1` should be a multiple of `x`.
// And for the same reason `x` can't be a multiple of `y` and therefore `p+1` should be a multiple of `y`.
// Possible values for `q+1` would be `x`, `2x`, `3x`, ... (and for `p+1`: `y`, `2y`, `3y`, ...)
// If I assume the lowest value `p+1=y` and `q+1=x` then the equation becomes
// `xy = y * x + x * y`
// `xy = 2xy` ==> contradition !
//
// Therefore `m = xy - x - y` actually can't be represented with some coefficients `m = px + qy`.
//
// With three numbers `x`,`y`,`z` and `gcd(x,y,z)=1` the idea is very similar:
// if there would be some coefficients `p`, `q` and `r` such that `m = pxy + qxz + ryz` represents `m = 2xyz - xy - xz - yz` then
// `(2xyz - xy - xz - yz) mod x = -yz`
// `pxy + qxz + ryz = 2xyz - xy - xz - yz`
// `2xyz = pxy + qxz + ryz + xy + xz + yz`
// `2xyz = (py+qz+y+z)x + (rz + z)y`
// Hence `rz + z = (r+1)z` must be a multiple of `x`. `z` can't be such a multiple (because of `gcd(x,y,z) = 1`).
// The same idea for `y` and `z` gives that `p+1` must be a multiple of `z` and `q+1` a multiple of `y`.
// As before - if I choose the smallest possible `p+1=z`, `q+1=y` and `r+1=x`:
// `2xyz = zxy + yxz + xyz + xy + xz + yz`
// `2xyz = 3xyz + xy + xz + yz` ==> contradiction
//
// I didn't come up with the full solution, I just know how to use search engines :-;
// I found the problem in the 24th International Mathemtical Olympiad held 1983 in Paris, France
// somewhat cryptic solution: http://www.cs.cornell.edu/~asdas/imo/imo/isoln/isoln833.html
// I stumbled across it while reading the German Wikipedia https://de.wikipedia.org/wiki/M%C3%BCnzproblem
// unfortunately, the English page misses that special case https://en.wikipedia.org/wiki/Coin_problem
// but it can be derived from their `n=2` explanations (pretty much what I have done above)

#include <iostream>
#include <vector>

int main()
{
  unsigned int limit = 5000;
  std::cin >> limit;

  // simple prime sieve from my toolbox
  std::vector<unsigned long long> primes = { 2 };
  for (unsigned int i = 3; i <= limit; i += 2)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto x : primes)
    {
      // prime is too large to be a divisor
      if (x*x > i)
        break;

      // divisible => not prime
      if (i % x == 0)
      {
        isPrime = false;
        break;
      }
    }

    // yes, we have a prime
    if (isPrime)
      primes.push_back(i);
  }

  // all combinations of primes
  unsigned long long sum = 0;
  for (size_t i = 0; i < primes.size(); i++)
    for (size_t j = i + 1; j < primes.size(); j++)
      for (size_t k = j + 1; k < primes.size(); k++)
      {
        auto p = primes[i];
        auto q = primes[j];
        auto r = primes[k];
        sum += 2*p*q*r - p*q - p*r - q*r;
      }

  std::cout << sum << std::endl;
  return 0;
}
