// ////////////////////////////////////////////////////////
// # Title
// Eight Divisors
//
// # URL
// https://projecteuler.net/problem=501
// http://euler.stephan-brumme.com/501/
//
// # Problem
// The eight divisors of 24 are 1, 2, 3, 4, 6, 8, 12 and 24.
// The ten numbers not exceeding 100 having exactly eight divisors are 24, 30, 40, 42, 54, 56, 66, 70, 78 and 88.
// Let `f(n)` be the count of numbers not exceeding `n` with exactly eight divisors.
// You are given `f(100) = 10`, `f(1000) = 180` and `f(10^6) = 224427`.
// Find `f(10^12)`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// When a number `n` is split into its prime factors `p_1`, `p_2`, ... and each prime factors is found `q_1`, `q_2`, ... times then
// `n = p_1^{q_1} * p_2^{q_2} * ...`
//
// The number of divisors is (see https://en.wikipedia.org/wiki/Divisor_function ):
// `d = (q_1 + 1) * (q_2 + 1) * ...`
//
// For `d = 8` there are only three possible combinations:
// `8 = 2 * 2 * 2 = 4 * 2 = 8 * 1`
//
// The first case means that `n` has three different prime factors `p_1`, `p_2` and `p_3` where:
// `q_1 = q_2 = q_3 = 1` when `n = p_1 * p_2 * p_3`
//
// To fulfil the second case, `n` needs exactly two different prime factor `p_1` and `p_2` where:
// `q_1 = 3` and `q_2 = 1` when `n = p_1^3 * p_2`
//
// And the third case applies `n` has only one prime factor `p_1`:
// `q_1 = 7` when `n = p_1^7`
//
// 24 has eight prime factors because of the second case: `24 = 2^3 * 3`.
//
// Case 1 needs the most time: two nested loops iterate over all prime numbers `p_1` and `p_2` where `p_1 < p_2`.
// Adding a third nested loop for `p_3` (with `p_2 < p_3`) would work but take forever.
// That's where the ''countPrimes(n)'' function comes into play: it returns the number of prime up to ''n''.
// To find the number of primes `p_3` for `p_2 < p_3 <= n`: `\#(p_3) = countPrimes(n / p_1 p_2) - countPrimes(p_2)`
//
// The second case is built on the same idea but needs only a single loops.
//
// There are just a few numbers for the third case and a simple loop without ''countPrimes'' suffices.
//
// I didn't write the ''countPrimes'' function - I found almost working code on http://am-just-a-nobody.blogspot.de/2015/11/c-code-for-primepi-function.html
// and improved it to match my needs. See my code comments for further details.
// In other programming languages it is often called ''PrimePi''.
//
// # Note
// You can still find my initial brute-force code (see ''hasEightDivisors''). It can easily solve for `n = 10^6` but is much too slow for `n = 10^12`.
//
// There are probably faster algorithm to count a number of primes. My program finishes in just under a minute but needs only 15 MByte RAM.
// This Wikipedia page lists a few alternatives: https://en.wikipedia.org/wiki/Prime-counting_function

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// ---------- brute-force code for small numbers, not used anymore ----------
// simple trial-division algorithm, too slow for large numbers
bool hasEightDivisors(unsigned long long n)
{
  unsigned int count = 2; // always at least two divisors: 1 and n
  for (unsigned long long i = 2; i*i <= n; i++)
    if (n % i == 0) // found a divisor ?
    {
      if (i*i == n)
        count++;    // perfect square
      else
        count += 2; // i is a divisor and n/i as well

      // abort early
      if (count > 8)
        return false;
    }

  // exactly 8 divisors ?
  return count == 8;
}

// ---------- and now the currently used code ----------

// return the number of primes up to n
// similar to PrimePi in popular math software
std::vector<unsigned int> primes; // will contain all primes up to sqrt(n)
unsigned long long countPrimes(unsigned long long n)
{
  // based on  http://am-just-a-nobody.blogspot.de/2015/11/c-code-for-primepi-function.html
  // algorithm http://am-just-a-nobody.blogspot.de/2015/11/algorithm-for-summing-all-primes-less.html
  // I don't have a thorough understanding of the code ...
  // but added these features:
  // - allocate only as much memory as actually needed
  // - fixed out-of-bounds errors
  // - remove the MOD and return the actual result
  // - made variables as local as possible
  // - find quick result for small n (when primes[] contains enough numbers)

  // if primes[] contains enough elements then run a fast binary search
  if (!primes.empty() && primes.back() > n)
  {
    // find smallest number larger than n
    auto i = std::upper_bound(primes.begin(), primes.end(), n);
    return std::distance(primes.begin(), i);
  }

  auto v = (unsigned int)sqrt(n);
  // about sqrt(n) * 12 bytes, for n = 10^12 => 12 MByte plus primes[]
  std::vector<unsigned long long> higher(v+2, 0);
  std::vector<unsigned int>       lower (v+2, 0);
  std::vector<bool>               used  (v+2, false);

  // assume all numbers are prime numbers
  unsigned long long result = n - 1;
  // the remaining lines subtract composites until result contains the number of primes

  // set up lower and upper bound
  for (unsigned int p = 2; p <= v; p++)
  {
    lower [p] = p - 1;
    higher[p] = n / p - 1;
  }

  for (unsigned int p = 2; p <= v; p++)
  {
    // composite ?
    if (lower[p] == lower[p - 1])
      continue;

    // store prime numbers (if not already existing)
    if (primes.empty() || p > primes.back())
      primes.push_back(p);

    auto temp  = lower[p - 1];
    // remove more composites
    result -= higher[p] - temp;

    auto pSquare = (unsigned long long)p * p;
    auto end = std::min<unsigned long long>(v, n / pSquare);
    // alternate between 1 and 2
    auto j = 1 + (p & 1);

    // adjust upper bound
    for (auto i = p + j; i <= end + 1; i += j)
    {
      if (used[i])
        continue;

      auto d = i * p;
      if (d <= v)
        higher[i] -= higher[d]    - temp;
      else
        higher[i] -= lower[n / d] - temp;
    }

    // adjust lower bound
    for (auto i = v; i >= pSquare; i--)
      lower[i] -= lower[i / p] - temp;

    // cross off multiples
    for (auto i = pSquare; i <= end; i += p*j)
      used[i] = true;
  }

  return result;
}

// count all primes with exactly 8 divisors
unsigned long long fast(unsigned long long n)
{
  // note: as a side effect, countPrimes generates all requires prime numbers
  //       which I need in the upcomig loops
  //       I don't actually care about the result of countPrimes(n) right now
  countPrimes(n); // a dedicated sieve might be faster, though

  // a * b * c where a < b < c
  unsigned long long countABC = 0;
  for (size_t indexA = 0; indexA < primes.size(); indexA++)
  {
    unsigned long long a = primes[indexA];
    if (a * a * a > n)
      break;

    for (size_t indexB = indexA + 1; indexB < primes.size(); indexB++)
    {
      auto b = primes[indexB];

      // min(c) = next prime after b
      // max(c) = last prime before n / (a*b)
      auto maxC = n / (a * b);
      if (maxC <= b)
        break;

      // count all primes between min(c) and max(c)
      auto high = countPrimes(maxC);
      auto low  = indexB + 1; // same as countPrimes(b);
      countABC += high - low;
    }
  }

  // a^3 * b
  unsigned long long countA3B = 0;
  for (auto a : primes)
  {
    auto maxB = n / ((unsigned long long)a * a * a);
    if (maxB <= 1)
      break;

    // b can be any prime
    auto numB = countPrimes(maxB);
    // but b must differ from a
    if (maxB >= a)
      numB--;

    countA3B += numB;
  }

  // a^7
  unsigned long long countA7 = 0;
  for (auto a : primes)
  {
    if ((unsigned long long) a * a * a * a * a * a * a > n)
      break;
    countA7++;
  }

  return countABC + countA3B + countA7;
}

int main()
{
  auto limit = 1000000000000ULL;
  std::cin >> limit;

  // find all number with naive approach (= extremely slow)
  //unsigned long long count = 0;
  //for (unsigned long long i = 1; i <= limit; i++)
  //  if (hasEightDivisors(i))
  //    count++;
  //std::cout << count << std::endl;

  std::cout << fast(limit) << std::endl;
  return 0;
}
