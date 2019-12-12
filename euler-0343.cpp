// ////////////////////////////////////////////////////////
// # Title
// Fractional Sequences
//
// # URL
// https://projecteuler.net/problem=343
// http://euler.stephan-brumme.com/343/
//
// # Problem
// For any positive integer `k`, a finite sequence `a_i` of fractions `dfrac{x_i}{y_i}` is defined by:
// `a_1 = dfrac{1}{k}` and
//
// `a_i = dfrac{x_{i-1}+1}{y_{i-1} - 1}` reduced to lowest terms for `i>1`.
//
// When `a_i` reaches some integer `n`, the sequence stops. (That is, when `y_i=1`.)
// Define `f(k) = n`.
// For example, for `k = 20`:
//
// `1/20 ==> 2/19 ==> 3/18 ==> 1/6 ==> 2/5 ==> 3/4 ==> 4/3 ==> 5/2 ==> 6/1 = 6`
//
// So `f(20) = 6`.
//
// Also `f(1) = 1`, `f(2) = 2`, `f(3) = 1` and `sum{f(k^3)} = 118937` for `1 <= k <= 100`.
//
// Find `sum{f(k^3)}` for `1 <= k <= 2 * 10^6`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// The ''bruteForce()'' function was written in a few minutes - and as expected is too slow for `2 * 10^6` but nevertheless proved to be a good help while debugging my faster algorithm:
// - start with ''numerator = 1'' and ''denominator = k'', then repeat until ''denominator == 1'':
// - add 1 to the ''numerator'' and subtract 1 from the ''denominator''
// - call ''gcd'' to check whether they share a factor, if yes, divide both by that factor
//
// I made a few observations:
// - the sum ''numerator + denominator'' remains constant if ''gcd == 1''
// - the ''gcd'' is always 1 if ''numerator + denominator'' is a prime number
// - if ''gcd > 1'' then ''gcd == numerator''
//
// That means the ''numerator'' will slowly increase from 1 to the smallest prime factor of ''numerator + denominator'', then reset to 1,
// then slowly increase to the next prime factor, etc.
// Since the initial value of ''numerator + denominator'' is `1 + k`, we are basically factorizing `1 + k`.
// In the very last cycle, ''numerator + denominator'' will be the largest prime factor of `1 + k`.
// Since the algorithm stops if ''denominator == 1'' I know that the last ''numerator'' is the largest prime factor of `1 + k`, minus 1.
//
// My function ''getMaxFactor()'' returns the largest prime factor of its argument.
// Therefore ''getMaxFactor(k + 1) - 1'' is equivalent to `f(k)`.
//
// One problem remains: factorization of large numbers is hard.
// Unfortunately, the problem statement asks for `k^3` where `k` can be `2 * 10^6` such that I have to factorize `8 * 10^18 + 1`
// (by the way: the largest prime factor of that number is 666667).
//
// Wolfram Alpha told me that `x^3 + 1` can be simplified:
// `x^3 + 1 = (x + 1) (x^2 - x + 1)`
// So instead of factorizing a single huge number `x^3 + 1` I can factorize two much smaller numbers.
// The largest prime factor found in `x + 1` or `x^2 - x + 1` is the largest prime factor of `x^3 + 1`.
//
// I tried several tricks to further speed up the factorization:
// - my standard prime sieve finds all primes below `2 * 10^6` ==> the largest factor of a prime is the prime itself
// - trial division using these primes
// - divide the current number by each factor to reduce it as fast as possible
// - if the reduced number becomes 1, then I'm finished
// - if the square of the current prime is larger than the reduced number, then the reduced number must be a prime
//
// However, the program still needed about 2.5 minutes (at least the result was correct ...).
// Desperately I search the web for factorization methods that are fast AND easy to implement.
//
// Fermat's factorization method was a nice candidate - but I can't measure any speedup (see https://en.wikipedia.org/wiki/Fermat%27s_factorization_method ).
// Maybe the ''getFermatFactors()'' turns out to be more useful in another Project Euler problem ... I keep it here for the sake of completeness.
//
// My experiments with Pollard's rho algorithm were a desaster: the program became much slower (maybe I had a bug somewhere, see https://en.wikipedia.org/wiki/Pollard%27s_rho_algorithm).
//
// Each computation of ''getMaxFactor'' is independent of each other. That's a perfect use case for multi-core CPUs !
// If you uncomment ''//#define PARALLEL'' then most modern C++ compilers (G++, Visual C++, Intel C++, etc.) will distribute the workload to all available CPUs.
// In my case I achieved an almost perfect performance boost: the solution is found 5.1 times faster on my 8-core computer (in about 28 seconds).
// I consider this speedup to be "almost linear" because my computer isn't a true 8-core system, it has 4 cores plus hyperthreading.

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// ---------- a few routines from my toolbox ----------

// greatest common divisor
template <typename T>
T gcd(T a, T b)
{
  while (a != 0)
  {
    T c = a;
    a = b % a;
    b = c;
  }
  return b;
}

// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;
unsigned int primeLimit = 0;

// return true, if x is a prime number
bool isSmallPrime(unsigned int x)
{
  // handle even numbers
  if ((x & 1) == 0)
    return x == 2;

  // lookup for odd numbers
  return sieve[x >> 1];
}

// find all prime numbers from 2 to size
void fillSieve(unsigned int size)
{
  // store only odd numbers
  const unsigned int half = (size >> 1) + 1;

  // allocate memory
  sieve.resize(half, true);
  // 1 is not a prime number
  sieve[0] = false;

  // process all relevant prime factors
  for (unsigned int i = 1; 2*i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3*i+1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2*i+1;
      }
    }
}

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
      result %= modulo;
      // skip b-- because the bit-shift at the end will remove the lowest bit anyway
    }

    // b is even ? a*b = (2*a)*(b/2)
    a <<= 1;
    a  %= modulo;

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
  // modified for problem 343: invoke isSmallPrime where appropriate
  if (p < primeLimit)
    return isSmallPrime(p);

  // IMPORTANT: requires mulmod(a, b, modulo) and powmod(base, exponent, modulo)

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

// brute-force: just follow the chain and reduce fractions as soon as possible
unsigned long long bruteForce(unsigned long long k)
{
  auto numerator   = 1ULL;
  auto denominator = k;

  // until the fraction is a natural number ...
  while (denominator != 1)
  {
    // figure out whether the fraction can be reduced
    auto sharedFactor = gcd(numerator, denominator);
    if (sharedFactor == 1)
    {
      // already a proper fraction, increase numerator and decrease denominator
      numerator++;
      denominator--;
    }
    else
    {
      // reduce to a proper fraction
      numerator   /= sharedFactor;
      denominator /= sharedFactor;
    }
  }

  return numerator;
}

// Fermat's factorization method
std::pair<unsigned long long, unsigned long long> getFermatFactors(unsigned long long n, unsigned int maxIterations)
{
  // https://en.wikipedia.org/wiki/Fermat%27s_factorization_method

  // trivial case: even numbers
  if (n % 2 == 0)
    return std::make_pair(2, n / 2);

  // already a perfect square ?
  auto x = (unsigned long long)sqrt(n);
  if (x * x == n)
    return std::make_pair(x, x);

  while (maxIterations-- > 0)
  {
    x++;
    auto y2 = x*x - n;

    // optimization: all perfect squares mod 16 are always 0,1,4,9
    // based on https://www.johndcook.com/blog/2008/11/17/fast-way-to-test-whether-a-number-is-a-square/
    auto mod16 = y2 % 16;
    if (mod16 == 0 || mod16 == 1 || mod16 == 4 || mod16 == 9)
    {
      // is y2 a perfect square ?
      auto y = (unsigned long long)sqrt(y2);
      if (y*y == y2)
        // yes, found factorization
        return std::make_pair(x - y, x + y);
    }
  }
  // aborted, no factors found
  return std::make_pair(1ULL, n);
}

std::vector<unsigned int> smallPrimes;

// return the largest prime factor, if x is prime then return x
unsigned long long getMaxFactor(unsigned long long x, unsigned long long minResult = 0)
{
  // no use in factorizing a prime :-)
  if (isPrime(x))
    return x;

  unsigned long long result = 1;
  auto reduce = x;
  for (auto factor : smallPrimes)
  {
    // at most one prime factor left ?
    if (factor * factor > reduce)
      break;

    // remove current prime factor
    bool foundFactor = false;
    while (reduce % factor == 0)
    {
      result  = std::max<unsigned long long>(result, factor);
      reduce /= factor;

      // abort search, prime factors will be too small
      if (reduce < minResult)
        return result;

      foundFactor = true;
    }

    if (foundFactor)
    {
      // only a prime left ?
      if (isPrime(reduce))
        break;

      // try Fermat's factorization
#define FERMAT
#ifdef  FERMAT
      // limit number of iterations
      auto fermat = getFermatFactors(reduce, 10);
      // found a factorization ?
      if (fermat.first > 1)
        return std::max(getMaxFactor(fermat.first), getMaxFactor(fermat.second));
#endif
    }
  }

  // x can be a prime, too
  return std::max(reduce, result);
}

int main()
{
  unsigned int limit = 2000000;
  std::cin >> limit;

  // generate enough primes
  primeLimit = limit + 100; // a few extra primes
  fillSieve(primeLimit);

  // copy to a dense array
  smallPrimes = { 2 };
  for (unsigned int i = 3; i <= primeLimit; i += 2)
    if (isPrime(i))
      smallPrimes.push_back(i);

  unsigned long long sum = 0;

#define PARALLEL
#ifdef  PARALLEL
#pragma omp parallel for reduction(+:sum) schedule(dynamic)
#endif
  for (long long i = 1; i <= limit; i++) // note: Visual C++ needs a signed data type
  {
    //auto slow = bruteForce(i * i * i);

//#define FACTORIZE_LARGE
#ifdef  FACTORIZE_LARGE
    // takes about 3x as long as the #else version
    auto factor = getMaxFactor(i * i * i + 1) - 1;
#else
    // i^3+1 = (i+1)(i^2-i+1)
    // getMaxFactor(i^3+1) = max(getMaxFactor(i+1), getMaxFactor(i^2-i+1))
    unsigned long long a = i + 1;
    unsigned long long b = i*i - i + 1;

    auto factor2 = getMaxFactor(b);
    // abort search early if no big factor possible
    auto factor1 = 1ULL;
    if (factor2 < a)
      factor1 = getMaxFactor(a, factor2);

    // choose the bigger factor
    auto factor = std::max(factor1, factor2);
#endif

    // remember: fast algorithm is getMaxFactor(k + 1) - 1 => I still have to subtract 1
    sum += factor - 1;
  }

  std::cout << sum << std::endl;
  return 0;
}
