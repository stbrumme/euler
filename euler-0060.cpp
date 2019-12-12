// ////////////////////////////////////////////////////////
// # Title
// Prime pair sets
//
// # URL
// https://projecteuler.net/problem=60
// http://euler.stephan-brumme.com/60/
//
// # Problem
// The primes 3, 7, 109, and 673, are quite remarkable. By taking any two primes and concatenating them in any order the result will always be prime.
// For example, taking 7 and 109, both 7109 and 1097 are prime. The sum of these four primes, 792, represents the lowest sum for a set of four primes with this property.
//
// Find the lowest sum for a set of five primes for which any two primes concatenate to produce another prime.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Let's skip forward to ''main'': a simple prime sieve finds all primes below 20000 (or whatever the user input is).
// Then each prime `a` is matched against each larger prime `b > a`: if `a` merged with `b` is prime __and__ `b` merged with ''a'' is prime, then `a` and `b` are a prime pair.
//
// When looking at my code, `a` is actually named ''smallPrime'' and `b` is named ''largePrime''.
// After calling the function ''match'' (see below) all primes pairs of ''smallPrime'' are stored in ''candidates''.
// Now each element of ''candidates'' definitely forms a prime pair with ''smallPrime''.
//
// The original problem asks for a set of five primes, but the Hackerrank problem extends this to 3, 4 and 5 primes.
// The functions ''checkTriple'', ''checkQuadruple'' and ''checkQuintuple'' follow the same idea:
// ''match'' each ''candidate'' against each other using nested loops. If 3, 4 or 5 primes match simultanueously, then insert the sum into ''sums''
// (which is a simple container keeping track of all results).
//
// At the end of the problem, ''sums'' is sorted and displayed.
//
// ==Helper functions==
// ''merge'' concatenate two numbers ''a'' and ''b'': the smallest `shift = 10^x` is found such that `shift > b`.
// Then the result ist `a * shift + b`. You can do the same pretty easy with strings but that's much slower.
// ''match'' checks whether the concatenated numbers ''ab'' and ''ba'' are prime (using the Miller-Rabin test I copied from problem 50).
//
// # Hackerrank
// As explained before, the prime sets may contain either (at least) 3, 4 or 5 primes.
// Note: __do not__ eliminate duplicate sums from the output. This was actually pretty unclear to me and took me quite some time to figure out.
//
// # Note
// That's the longest code I had to write for a Project Euler problem so far !
// (at least I could copy about 50% from previous problems ...)

#include <vector>
#include <iostream>
#include <algorithm>

// will be the result
std::vector<unsigned int> sums;

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
#ifdef __GNUC__
  // use GCC's optimized 128 bit code
  return ((unsigned __int128)a * b) % modulo;
#endif

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

// return (base^exponent) % modulo
unsigned long long powmod(unsigned long long base, unsigned long long exponent, unsigned long long modulo)
{
  unsigned long long result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = mulmod(result, base, modulo);

    // even exponent ? a^b = (a*a)^(b/2)
    base = mulmod(base, base, modulo);
    exponent >>= 1;
  }
  return result;
}

// Miller-Rabin-test
bool isPrime(unsigned long long p)
{
  // some code from             https://ronzii.wordpress.com/2012/03/04/miller-rabin-primality-test/
  // with optimizations from    http://ceur-ws.org/Vol-1326/020-Forisek.pdf
  // good bases can be found at http://miller-rabin.appspot.com/

  // trivial cases
  const unsigned int bitmaskPrimes2to31 = (1 <<  2) | (1 <<  3) | (1 <<  5) | (1 <<  7) |
                                          (1 << 11) | (1 << 13) | (1 << 17) | (1 << 19) |
                                          (1 << 23) | (1 << 29); // = 0x208A28Ac
  if (p < 31)
    return (bitmaskPrimes2to31 & (1 << p)) != 0;

  if (p %  2 == 0 || p %  3 == 0 || p %  5 == 0 || p % 7 == 0 || // divisible by a small prime
      p % 11 == 0 || p % 13 == 0 || p % 17 == 0)
    return false;

  if (p < 17*19) // we filtered all composite numbers < 17*19, all others below 17*19 must be prime
    return true;

  // test p against those numbers ("witnesses")
  // good bases can be found at http://miller-rabin.appspot.com/
  const unsigned int STOP = 0;
  const unsigned int TestAgainst1[] = { 377687, STOP };
  const unsigned int TestAgainst2[] = { 31, 73, STOP };
  const unsigned int TestAgainst3[] = { 2, 7, 61, STOP };
  // first three sequences are good up to 2^32
  const unsigned int TestAgainst4[] = { 2, 13, 23, 1662803, STOP };
  const unsigned int TestAgainst7[] = { 2, 325, 9375, 28178, 450775, 9780504, 1795265022, STOP };

  // good up to 2^64
  const unsigned int* testAgainst = TestAgainst7;
  // use less tests if feasible
  if (p < 5329)
    testAgainst = TestAgainst1;
  else if (p < 9080191)
    testAgainst = TestAgainst2;
  else if (p < 4759123141ULL)
    testAgainst = TestAgainst3;
  else if (p < 1122004669633ULL)
    testAgainst = TestAgainst4;

  // find p - 1 = d * 2^j
  auto d = p - 1;
  d >>= 1;
  unsigned int shift = 0;
  while ((d & 1) == 0)
  {
    shift++;
    d >>= 1;
  }

  // test p against all bases
  do
  {
    auto x = powmod(*testAgainst++, d, p);
    // is test^d % p == 1 or -1 ?
    if (x == 1 || x == p - 1)
      continue;

    // now either prime or a strong pseudo-prime
    // check test^(d*2^r) for 0 <= r < shift
    bool maybePrime = false;
    for (unsigned int r = 0; r < shift; r++)
    {
      // x = x^2 % p
      // (initial x was test^d)
      x = powmod(x, 2, p);
      // x % p == 1 => not prime
      if (x == 1)
        return false;

      // x % p == -1 => prime or an even stronger pseudo-prime
      if (x == p - 1)
      {
        // next iteration
        maybePrime = true;
        break;
      }
    }

    // not prime
    if (!maybePrime)
      return false;
  } while (*testAgainst != STOP);

  // prime
  return true;
}

// merge two numbers, "append their digits"
unsigned long long merge(unsigned long long a, unsigned long long b)
{
  // merge(12, 34) => 1234
  unsigned long long shift = 10;
  while (shift <= b)
    shift *= 10;
  return a * shift + b;
}

// true if a and b can be merged in any way and the result is still a prime
bool match(unsigned long long a, unsigned long long b)
{
  return isPrime(merge(a, b)) && isPrime(merge(b, a));
}

// find all triplets:
// all numbers in "candidates" already match with first
// now we have to check every numbers in "candidates" against each other
void checkTriple(unsigned int first, const std::vector<unsigned int>& candidates)
{
  for (size_t index2 = 0; index2 < candidates.size(); index2++)
    for (size_t index3 = index2 + 1; index3 < candidates.size(); index3++)
      // match ?
      if (match(candidates[index2], candidates[index3]))
      {
        // append sum to result set
        auto sum = first + candidates[index2] + candidates[index3];
        sums.push_back(sum);
      }
}

// find all quadruples, same idea as checkTriple, but this time 1+3 number must match
void checkQuadruple(unsigned int first, const std::vector<unsigned int>& candidates)
{
  for (size_t index2 = 0; index2 < candidates.size(); index2++)
    for (size_t index3 = index2 + 1; index3 < candidates.size(); index3++)
    {
      // not even a triple ?
      if (!match(candidates[index2], candidates[index3]))
        continue;

      // match fourth number
      for (size_t index4 = index3 + 1; index4 < candidates.size(); index4++)
        if (match(candidates[index2], candidates[index4]) &&
            match(candidates[index3], candidates[index4]))
        {
          // append sum to result set
          auto sum = first + candidates[index2] + candidates[index3] + candidates[index4];
          sums.push_back(sum);
        }
    }
}

// find all quintuples, same idea as above, just nested one level deeper
void checkQuintuple(unsigned int first, const std::vector<unsigned int>& candidates)
{
  for (size_t index2 = 0; index2 < candidates.size(); index2++)
    for (size_t index3 = index2 + 1; index3 < candidates.size(); index3++)
    {
      // not even a triple ?
      if (!match(candidates[index2], candidates[index3]))
        continue;

      for (size_t index4 = index3 + 1; index4 < candidates.size(); index4++)
      {
        // not even a quadruple ?
        if (!match(candidates[index2], candidates[index4]) ||
            !match(candidates[index3], candidates[index4]))
          continue;

        // match fifth number
        for (size_t index5 = index4 + 1; index5 < candidates.size(); index5++)
          if (match(candidates[index2], candidates[index5]) &&
              match(candidates[index3], candidates[index5]) &&
              match(candidates[index4], candidates[index5]))
          {
            // append sum to result set
            auto sum = first + candidates[index2] + candidates[index3] +
                               candidates[index4] + candidates[index5];
            sums.push_back(sum);
          }
      }
    }
}

int main()
{
  // all primes are below this threshold
  unsigned int maxPrime = 20000;
  // number of primes in a group
  unsigned int size     =     5;
  std::cin >> maxPrime >> size;

  // all primes that can be part of a result set
  std::vector<unsigned int> primes;
  // find all primes up to n (20000 at most)
  // note: 2 is deliberately excluded because "any combination must be a prime"
  // => but any number where we concat 2 to the end can't be prime
  for (unsigned int i = 3; i < maxPrime; i += 2)
  {
    bool isPrime = true;
    for (auto p : primes)
    {
      if (p*p > i)
        break;
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    if (isPrime)
      primes.push_back(i);
  }

  for (size_t i = 0; i < primes.size(); i++)
  {
    auto smallPrime = primes[i];
    // no prime number ends with 5 (except 5 itself) => just a simple performance tweak
    if (smallPrime == 5)
      continue;

    // find all larger primes that can be paired with "smallPrime"
    std::vector<unsigned int> candidates;
    for (size_t j = i + 1; j < primes.size(); j++)
    {
      auto largePrime = primes[j];
      if (match(smallPrime, largePrime))
        candidates.push_back(largePrime);
    }

    // all other candidates must be "pairable" to each other, too
    if (size == 3)
      checkTriple(smallPrime, candidates);
    else if (size == 4)
      checkQuadruple(smallPrime, candidates);
    else // size == 5
      checkQuintuple(smallPrime, candidates);
  }

  // print all sums in ascending order, some sums may occur multiple times
  std::sort(sums.begin(), sums.end());
  for (auto s : sums)
    std::cout << s << std::endl;
}
