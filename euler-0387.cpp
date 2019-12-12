// ////////////////////////////////////////////////////////
// # Title
// Harshad Numbers
//
// # URL
// https://projecteuler.net/problem=387
// http://euler.stephan-brumme.com/387/
//
// # Problem
// A Harshad or Niven number is a number that is divisible by the sum of its digits.
// 201 is a Harshad number because it is divisible by 3 (the sum of its digits.)
// When we truncate the last digit from 201, we get 20, which is a Harshad number.
// When we truncate the last digit from 20, we get 2, which is also a Harshad number.
// Let's call a Harshad number that, while recursively truncating the last digit, always results in a Harshad number a right truncatable Harshad number.
//
// Also:
// 201/3=67 which is prime.
// Let's call a Harshad number that, when divided by the sum of its digits, results in a prime a strong Harshad number.
//
// Now take the number 2011 which is prime.
// When we truncate the last digit from it we get 201, a strong Harshad number that is also right truncatable.
// Let's call such primes strong, right truncatable Harshad primes.
//
// You are given that the sum of the strong, right truncatable Harshad primes less than 10000 is 90619.
//
// Find the sum of the strong, right truncatable Harshad primes less than `10^14`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// A large portion of my code consists of the Miller-Rabin prime test and ''digitSum'' from my [toolbox](../toolbox/).
//
// All single-digit numbers are Harshad numbers.
// I generate all Harshad numbers with `n` digits by appending each possible digit 0 to 9 to all Harshad numbers with `n-1` digits.
// All strong Harshad numbers are stored in ''strong''. If the `n-1` digit number (I call it the "parent") is strong and the current number is prime, then I add it to the total sum.

#include <iostream>
#include <vector>
#include <algorithm>

// ---------- mulmod, powmod and Miller-Rabin test from my toolbox ----------

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

// return (base^exponent) % modulo => simple implementation
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
  const unsigned int bitmaskPrimes2to31 = (1 <<  2) | (1 <<  3) | (1 <<  5) | (1 <<  7) |
                                          (1 << 11) | (1 << 13) | (1 << 17) | (1 << 19) |
                                          (1 << 23) | (1 << 29); // = 0x208A28Ac
  if (p < 31)
    return (bitmaskPrimes2to31 & (1 << p)) != 0;

  if (p %  2 == 0 || p %  3 == 0 || p %  5 == 0 || p % 7 == 0 || // divisible by a small prime
      p % 11 == 0 || p % 13 == 0 || p % 17 == 0)
    return false;

  if (p < 17 * 19) // we filtered all composite numbers < 17*19, all others below 17*19 must be prime
    return true;

  // fine-tuning for the problem:
  // fast check of more small primes (up to 100)
  if (p % 19 == 0 || p % 23 == 0 || p % 29 == 0 || p % 31 == 0 || p % 37 == 0 ||
      p % 41 == 0 || p % 43 == 0 || p % 47 == 0 || p % 53 == 0 || p % 59 == 0 ||
      p % 61 == 0 || p % 67 == 0 || p % 71 == 0 || p % 73 == 0 || p % 79 == 0 ||
      p % 83 == 0 || p % 89 == 0 || p % 97 == 0)
    return false;

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

// ---------- now code specific to problem 387 ----------

// sum of all digits
unsigned int digitSum(unsigned long long x)
{
  unsigned int result = 0;
  while (x >= 10)
  {
    result += x % 10;
    x      /= 10;
  }
  return result + x;
}

int main()
{
  // up to 10^14
  unsigned int maxDigits = 14;
  std::cin >> maxDigits;

  // strong, right-truncatable Harshad numbers
  unsigned long long result = 0;

  // all single-digit numbers are Harshad numbers
  std::vector<unsigned long long> todo = { 1,2,3,4,5,6,7,8,9 };

  // strong Harshad numbers
  std::vector<unsigned long long> strong;

  // find all Harshad numbers with current number of digits
  unsigned int numDigits = 1;
  while (numDigits < maxDigits)
  {
    numDigits++;
    std::vector<unsigned long long> next;

    // process all Harshad numbers of the previous iteration
    for (auto x : todo)
    {
      // "strong" is a sorted vector
      bool isStrongParent = std::binary_search(strong.begin(), strong.end(), x);

      // append every possible digit
      for (unsigned int digit = 0; digit <= 9; digit++)
      {
        auto current = x * 10 + digit;
        // prime and strong Harshad number of last digit is removed ?
        if (isStrongParent && isPrime(current))
          result += current;

        // divisible by its digital sum ?
        auto reduced = digitSum(current);
        if (current % reduced == 0)
        {
          // Harshad number ?
          next.push_back(current);

          // even a strong Harshad number ?
          if (isPrime(current / reduced))
            strong.push_back(current);
        }
      }
    }

    todo = std::move(next);
  }

  // and we're done
  std::cout << result << std::endl;
  return 0;
}
