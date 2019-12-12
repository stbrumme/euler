// ////////////////////////////////////////////////////////
// # Title
// Numbers for which Euler's totient function equals 13!
//
// # URL
// https://projecteuler.net/problem=248
// http://euler.stephan-brumme.com/248/
//
// # Problem
// The first number n for which `phi(n)=13!` is `6227180929`.
//
// Find the 150,000th such number.
//
// # Solved by
// Stephan Brumme
// November 2017
//
// # Algorithm
// There are two basic properties of Euler's totient function `phi(x)` (see https://en.wikipedia.org/wiki/Euler%27s_totient_function ):
// (1) `phi(p) = p - 1` if `p` is a prime
// (2) `phi(a b) = phi(a) phi(b)` if `a` and `b` are coprime
//
// Every number can be split uniquely into its prime factors.
// My program transforms `phi(x) = 13!` such that `phi(x) = phi(d_1 * d_2 * ... * d_n) = phi(d_1) * phi(d_2) * ... * phi(d_n) = D_1 * D_2 * ... * D_n`.
//
// If I find all divisors of 13! and partition them in such a way that `13! = D_1 * D_2 * ... D_n` I can apply rule (1) and/or (2).
// For example, if for some divisor I can show that `D_i + 1` is prime then according to (1):
// (3) `D_i + 1 = phi(p_1) + 1 = (p_i - 1) = p_i`
// So I have found a prime factor `D_i + 1` of a potential number `n` where `phi(n) = 13!`
//
// Factorizing 13! gives us:
// (4) `13! = 2^10 * 3^5 * 5^2 * 7 * 11 * 13`
//
// Now I iterate over all divisors of 13! and check whether `D_i + 1` is prime (see ''findCandidates'').
// All those numbers are stored in the container ''candidates''.
//
// Then I combine the candidates until the product of their totients yields 13! (see ''search'').
// By the way: I needed a few minutes to realize that some candidates can appear multiple times in the product.
//
// The problem statement says that 6227180929 is the smallest number where `phi(x) = 13!`:
// (5) `6227180929 = 66529 * 93601 = (66528 + 1) * (93600 + 1)`
// (6) `66528 = 2^5 * 3^3 * 7 * 11`
// (7) `93600 = 2^5 * 3^2 * 5^2 * 13`
// (8) `66528 * 93600 = 6227020800 = 13!`
//
// And indeed, my program finds 13!'s divisors 66528 and 93600 (among others) because their successor is a prime and combines them.
//
// Sorting those roughly 180000 numbers finishes almost instantly but I went the crazy way and preferred
// ''std::nth_element'' because it is a little bit more efficient - but you can't measure the performance advantage ...
//
// # Note
// A huge part of my code consists of my standard Miller-Rabin prime test which I just copied from my [toolbox](/toolbox/).

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

// ---------- Miller-Rabin prime test from my toolbox ----------

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
  // IMPORTANT: requires mulmod(a, b, modulo) and powmod(base, exponent, modulo)

  // some code from             https://ronzii.wordpress.com/2012/03/04/miller-rabin-primality-test/
  // with optimizations from    http://ceur-ws.org/Vol-1326/020-Forisek.pdf
  // good bases can be found at http://miller-rabin.appspot.com/

  // trivial cases
  const unsigned int bitmaskPrimes2to31 = (1 << 2) | (1 << 3) | (1 << 5) | (1 << 7) |
    (1 << 11) | (1 << 13) | (1 << 17) | (1 << 19) |
    (1 << 23) | (1 << 29); // = 0x208A28Ac
  if (p < 31)
    return (bitmaskPrimes2to31 & (1 << p)) != 0;

  if (p % 2 == 0 || p % 3 == 0 || p % 5 == 0 || p % 7 == 0 || // divisible by a small prime
    p % 11 == 0 || p % 13 == 0 || p % 17 == 0)
    return false;

  if (p < 17 * 19) // we filtered all composite numbers < 17*19, all others below 17*19 must be prime
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
      x = mulmod(x, x, p);
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

// ---------- problem-specific code ----------

// will be 13!
unsigned long long factorial = 1;
// factorized 13! such such factors[2] = 10; etc.
std::map<unsigned int, unsigned int> factors;
// all primes where prime-1 is a divisor of 13!
std::set<unsigned int> candidates;
// all x where phi(x) = 13!
std::vector<unsigned long long> results;

// "build" all numbers using the prime factors in "factors"
// if number + 1 is prime then store as a candidate
void findCandidates(unsigned long long number = 1, unsigned int lastPrime = 1)
{
  // all primes consumed ?
  auto nextPrime = factors.upper_bound(lastPrime);
  if (nextPrime == factors.end())
  {
    // check if successor is prime
    if (isPrime(number + 1))
      candidates.insert(number + 1);
    return;
  }

  // process all combinations recursively
  auto base     = nextPrime->first;
  auto exponent = nextPrime->second;
  for (unsigned int i = 0; i <= exponent; i++)
  {
    findCandidates(number, base);
    number *= base;
  }
}

// look for all number with phi(x) = 13!
void search(unsigned long long number = 1, unsigned long long phi = 1,
            unsigned int largestPrime = 1)
{
  // find next prime (or increase the exponent of the latest prime)
  auto from = candidates.lower_bound(largestPrime);
  for (auto i = from; i != candidates.end(); i++)
  {
    // current prime
    auto current = *i;

    // include one more prime factor
    auto nextNumber = number * current;

    // phi(prime)   = prime - 1
    auto nextPhi = phi * (current - 1);
    // phi(prime^x) = phi(prime^(x-1)) * prime
    if (current == largestPrime)
      nextPhi += phi; // phi * current = phi * (current - 1) + phi

    // too large ?
    if (nextPhi >  factorial)
      break;
    // match ?
    if (nextPhi == factorial)
    {
      results.push_back(nextNumber);
      break;
    }

    // keep going if 13! is still reachable
    if (factorial % nextPhi == 0)
      search(nextNumber, nextPhi, current);
  }
}

int main()
{
  unsigned int thirteen = 13;
  unsigned int index = 150000;
  std::cin >> thirteen >> index;

  // compute 13!
  factorial = 1;
  for (unsigned int i = 2; i <= thirteen; i++)
    factorial *= i;

  // and factorize it
  auto reduce = factorial;
  for (unsigned int i = 2; i <= thirteen; i++)
    while (reduce % i == 0)
    {
      factors[i]++;
      reduce /= i;
    }

  // generate all primes whose predecessor is a divisor of 13!
  findCandidates();

  // find all numbers with eulerphi() = 13!
  search();

  // array is zero-indexed
  index--;
  // partial sort: only need the 150000th to be at its correct position
  auto pos = results.begin() + index;
  std::nth_element(results.begin(), pos, results.end());
  // (I could have used std::sort as well, but std::nth_element is a tiny bit faster in this case)
  std::cout << *pos << std::endl;
  return 0;
}
