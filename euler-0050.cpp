// ////////////////////////////////////////////////////////
// # Title
// Consecutive prime sum
//
// # URL
// https://projecteuler.net/problem=50
// http://euler.stephan-brumme.com/50/
//
// # Problem
// The prime 41 can be written as the sum of six consecutive primes:
// 41 = 2 + 3 + 5 + 7 + 11 + 13
//
// This is the longest sum of consecutive primes that adds to a prime below one-hundred.
// The longest sum of consecutive primes below one-thousand that adds to a prime, contains 21 terms, and is equal to 953.
//
// Which prime, below one-million, can be written as the sum of the most consecutive primes?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The basic idea is pretty simple:
// - generate a ton of prime numbers `p`
// - for each sum `sum_{x=i..j}{p_x}` perform a primality test
// - print the maximum sum that is prime
//
// Initially I struggled a little bit to find a fast solution (especially fast enough for Hackerrank, where the sum may be up to `10^12`).
// Then my first brute force code revealed a few observations:
// - if the sum of the first `n` primes is prime, then maybe the sum of the first `n+1` primes isn't
// - if the sum of the first `n` primes isn't prime, then maybe the sum of the first `n+1` primes is
// ==> just keep going, no matter how many non-prime sums we have seen, eventually the sum will be prime again
//
// Sometimes starting with the first prime `2` doesn't produce the highest sum. The problems mentions `953` which is `7 + 9 + 11 + ... + 89`.
// The surprising fact is that all "best" chains below `10^12` start with at most `131` (!). I can't explain why - that's just what I saw in my output !
//
// My code generates prime numbers on-demand. Whenever the main loop runs out of primes, it calls ''morePrimes(x)'' which ensures that ''primes'' will contain at ''x'' prime numbers.
// On top of that, ''primeSum[i]'' is the sum of the first ''i'' prime numbers (zero-based index), e.g. ''primeSum[2] = 2+3+5 = 10''.
//
// The interesting fact about ''primeSum'' is that the sum of the first `x` prime numbers excluding the initial `y` primes is ''primeSum[x] - primeSum[y]''.
// For example, ''primeSum[23] - primeSum[2] = 963 - 10 = 953'', that means there is chain containing `23-2=21` elements with a sum of `953`.
//
// A simple loop finds the largest sum which is below the target: ''primeSum[545] = 997661''
// If that number isn't prime (`997661 = 7 * 359 * 397`), then we look at its predecessor ''primeSum[544]'' and so on - until the sum is prime.
// As I explained earlier, the best chain maybe doesn't start with the first prime.
// Therefore we have to check ''primeSum[545] - primeSum[0] = 997659'' as well, then try ''primeSum[544] - primeSum[0]'', ...
// until we arrive at ''primeSum[545] - primeSum[31]'' because ''primes[31] = 131''.
//
// There are simple primality tests for such small number but they all fall apart when the sum is large (such as `10^12` in the Hackerrank version).
// Take a look at my [toolbox](../toolbox/#primetests) for inspiration.
//
// # Hackerrank
// It took my quite a while to come up with a fast and stable prime test.
// Searching on the internet immediately brings up the Miller-Rabin test: https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
//
// Unfortunately, most C/C++ implementations either can't handle 64 bit numbers properly or are way to complex to fit in a few lines of code.
// That's why had to write my own routine (of course inspired by looking at other sources).
//
// Modular arithmetic was already used in problem 48, please see there for an explanation of ''mulmod'' and ''powmod''.
// My [toolbox](../toolbox/#primetests) contains code for a 32 bit Miller-Rabin test where those two functions can be written in a much simpler way.
//
// # Note
// I have to admit that the mathematics of the Miller-Rabin test is not easy to understand for a non-mathematican like me:
// I couldn't have written my code without these sources of inspiration:
// - some code from             https://ronzii.wordpress.com/2012/03/04/miller-rabin-primality-test/
// - with optimizations from    http://ceur-ws.org/Vol-1326/020-Forisek.pdf
// - good bases can be found at http://miller-rabin.appspot.com/
// - 32 bit C code              https://de.wikipedia.org/wiki/Miller-Rabin-Test

#include <iostream>
#include <vector>

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
  // fast path
  if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    return (a * b) % modulo;

  // we might encounter overflows (slow path)
  unsigned long long result = 0;
  unsigned long long factor = a % modulo;

  // bitwise multiplication
  while (b > 0)
  {
    // b is odd ? a*b = a + a*(b-1)
    if (b & 1)
    {
      result += factor;
      if (result >= modulo)
        result %= modulo;
    }

    // b is even ? a*b = (2*a)*(b/2)
    factor <<= 1;
    if (factor >= modulo)
      factor %= modulo;

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

std::vector<unsigned int>       primes;
std::vector<unsigned long long> primeSum;

// make sure that at least "num" primes are available in "primes"
void morePrimes(unsigned int num)
{
  if (primes.empty())
  {
    primes  .reserve(400000);
    primeSum.reserve(400000);

    primes.push_back(2);
    primes.push_back(3);

    primeSum.push_back(2);
  }

  for (auto i = primes.back() + 2; primes.size() <= num; i += 2)
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

  for (auto i = primeSum.size(); i < primes.size(); i++)
    primeSum.push_back(primeSum.back() + primes[i]);
}

int main()
{
  // generate some primes
  const unsigned int PrimesPerBatch = 10000;
  morePrimes(PrimesPerBatch);

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long last = 1000000;
    std::cin >> last;

    unsigned long long best = 2; // highest prime sum
    unsigned int maxLength  = 0; // longest chain (must add plus one)

    // all sequences start with surprisingly small prime numbers
    // a brute-force search showed that all "good" chains start at 2..131
    unsigned int start = 0; // primes[0] = 2
    while (primes[start] <= 131 && primes[start] <= last)
    {
      unsigned long long subtract = 0;
      if (start > 0)
        subtract = primeSum[start - 1];

      unsigned int pos = start + maxLength;
      // find shortest chain whose sum exceeds the limit
      while (primeSum[pos] - subtract <= last)
      {
        pos++;
        // running out of prime numbers ? add more !
        if (pos + 100 >= primes.size()) // plus 100 is probably too cautious
          morePrimes(primes.size() + PrimesPerBatch);
      }
      pos--;

      // chop off one prime number until the sum is prime, too
      while (pos - start > maxLength)
      {
        unsigned long long sum = primeSum[pos] - subtract;
        // yes, we have a good candidate (maybe better ones for other values of "start", though)
        if (isPrime(sum))
        {
          maxLength = pos - start;
          best = sum;
          break;
        }

        pos--;
      }

      start++;
    }

    // if sum is > 0 then "length" didn't count the first element
    if (best >= 2)
      maxLength++;
    std::cout << best << " " << maxLength << std::endl;
  }

  return 0;
}
