// ////////////////////////////////////////////////////////
// # Title
// Digital Signature
//
// # URL
// https://projecteuler.net/problem=290
// http://euler.stephan-brumme.com/290/
//
// # Problem
// How many integers `0 <= n < 10^18` have the property that the sum of the digits of `n` equals the sum of digits of `137n`?
//
// # Solved by
// Stephan Brumme
// November 2017
//
// # Algorithm
// A brute-force algorithm can be written without much thinking (and just stealing the ''digitSum()'' function from problem 119).
// It works pretty well for `n <= 10^8`.
// There is a small optimization which I added based on the observation:
// if `n` is __not__ a multiple of 9 then the sum of digits of `n` never matches the sum of digits of `137n`.
//
// Coming up with a faster algorithm took me two months - well, I was working on other problems in the meantime, but this problem was a tough nut to crack -
// escpecially considering the short solution that I finally found. Note: apparently almost everyone found the same algorithm, the fastest solver needed just 7 minutes (!) according to the forum.
//
// While the brute-force algorithm works for every `n`, the faster algorithm requires `n` to be a power of ten, that is `n = 10^k`, e.g. `n = 10^18`.
//
// Let's say `S(n)` returns the sum of digits of `n`. Then I have to find all `n < 10^18` where
// (1) `S(n) = S(137n)`
//
// If I partition `n` into `n = 10x + y` then `y` is the last digit of `n` and `x` everything else.
// (2) `S(n) = S(10x + y) = S(x) + y`
//
// Combining (1) and (2):
// (3) `S(x) + y = S(137 * (10x + y))`
// (4) `S(x) + y = S(137 * 10x + 137y)`
//
// If I partition `137y` into `137y = 10u + v` then
// (5) `u = \lfloor dfrac{137y}{10} \rfloor`
// (6) `v = 137y mod 10`
// (7) `S(x) + y = S(137 * 10x + 10u + v)`
// (8) `S(x) + y = S(137x + u) + v`
//
// If `n < 10`, that means the number of digits is 1, then `x = 0` and (8) simplifies accordingly.
// If `n >= 10` then `x` has one digit less than `n` (it's missing the right-most digit which is `y`) and
// (9) `S(137n) = S(137x + u) + v`
// So that the recursion pattern is
// (10) `u_{next} = \lfloor dfrac{137u}{10} \rfloor`
// (11) `v_{next} = 137u mod 10 + v - digit`
//
// # Note
// I didn't find the final steps __not__ by sitting in front of a piece of paper - instead it was laborous work fitting the equations to the results of my brute-force search.
// As mentioned before, several solvers went the straight way and immediately found the correct algorithm. Not me, I was stupid ...
//
// Usually I rely on ''std::map'' for caching because it's simple:
// - no need to pre-allocate memory and fill it with an ''Unknown'' indicator
// - works great when the ''id''s have huge gaps inbetween
// It solves the current problem in 0.415 seconds.
//
// When I have the correct solution, I often try to speed up the program because ''std::map'' involves lots of calls to ''new'' and ''delete''.
// An ''std::unordered_map'' is a plug-in replacement. Since most hash maps are based on a big blocks of continuous memory, there are less ''new'' and ''delete''.
// That small difference (combined with better locality / CPU caching) is enough to solve the current problem almost three times faster: in only 0.120 seconds.
//
// There are no huge gaps between ''id''s so that I converted the ''cache'' container to ''std::vector'' and finish in just 0.027 seconds, again 4 times faster than ''std::unordered_map''.
// Even more important, each ''new'' consumes a few bytes on its own. Therefore the faster ''std::vector'' version needs 50% less memory, too.
// Feel free to experiment with ''#define FAST''.

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <cmath>

// compare x and 137x
const unsigned int Signature = 137;

// copied from problem 119
unsigned int digitSum(unsigned long long x)
{
  unsigned int result = 0;
  while (x > 0)
  {
    result += x % 10;
    x      /= 10;
  }
  return result;
}

// the brute-force algorithm is surprisingly fast for limit < 10^8
unsigned long long bruteForce(unsigned long long limit)
{
  unsigned long long count = 0;
  for (unsigned long long i = 0; i < limit; i += 9) // observation: always multiples of 9
    if (digitSum(i) == digitSum(i * Signature))
      count++;

  return count;
}

// different container types:
// std::vector:        0.027s
// std::unordered_map: 0.120s
// std::map:           0.415s
#define FAST
#ifdef  FAST
const long long Unknown = -1;
std::vector<long long> cache((18+1) * 137 * 240, Unknown); // see search() for the constants
#else
std::unordered_map<unsigned int, long long> cache;
//std::map<unsigned int, long long> cache;
#endif

// count all numbers where digitSum(x) == digitSum(137x)
unsigned long long search(unsigned int numDigits, unsigned int u = 0, int v = 0)
{
  // memoize
  unsigned int id = numDigits;
  id *= 137;
  id += u;       // 0..136
  id *= 240;
  id += v + 120; // -116..+116
#ifdef FAST
  if (cache[id] != Unknown)
    return cache[id];
#else
  auto lookup = cache.find(id);
  if (lookup != cache.end())
    return lookup->second;
#endif

  // only one digit left ?
  if (numDigits == 1)
  {
    // digitSum(137d + u) must be identical to digit + v
    // check every possible digit 0..9
    unsigned int result = 0;
    for (unsigned int digit = 0; digit <= 9; digit++)
      if (digitSum(Signature * digit + u) + v == digit)
        result++;
    cache[id] = result;
    return result;
  }

  // "shave off" one digit and continues with recursion ...
  unsigned long long result = 0;
  for (auto digit = 0; digit <= 9; digit++)
  {
    auto nextU = (Signature * digit + u) / 10;
    auto nextV = (Signature * digit + u) % 10 + v - digit;
    result += search(numDigits - 1, nextU, nextV);
  }

  cache[id] = result;
  return result;
}

int main()
{
  // analyze all numbers below 10^18
  unsigned int limit = 18;
  std::cin >> limit;

  //std::cout << bruteForce(pow(10, limit)) << std::endl;
  std::cout << search(limit) << std::endl;
  return 0;
}
