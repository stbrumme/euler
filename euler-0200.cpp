// ////////////////////////////////////////////////////////
// # Title
// Find the 200th prime-proof sqube containing the contiguous sub-string "200"
//
// # URL
// https://projecteuler.net/problem=200
// http://euler.stephan-brumme.com/200/
//
// # Problem
// We shall define a sqube to be a number of the form, `p^2 q^3`, where `p` and `q` are distinct primes.
// For example, `200 = 5^2 2^3` or `120072949 = 23^2 61^3`.
//
// The first five squbes are 72, 108, 200, 392, and 500.
//
// Interestingly, 200 is also the first number for which you cannot change any single digit to make a prime; we shall call such numbers, prime-proof.
// The next prime-proof sqube which contains the contiguous sub-string "200" is 1992008.
//
// Find the 200th prime-proof sqube containing the contiguous sub-string "200".
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// I need two things:
// - a fast primality test, suitable for moderately large numbers ==> Miller-Rabin test from my [toolbox](/toolbox/).
// - a sorted container storing candidates in ascending order
//
// My struct ''Sqube'' represents a number `p^2 q^3`. It automatically computes ''value == p*p * q*q*q''.
// Due to its member function ''operator<'', I can insert it into a standard ''std::set'' where the left-most element will be the smallest sqube (at ''squbes.begin()'').
// The ''main()'' function starts with the two squbes ''{ 2, 3 }'' and ''{ 3, 2 }''. Whenever a sqube has been processed, its "successors" ''{ p+1, q }'' and ''{ p, q+1 }'' will be added to the ''set''.
// (but avoid adding a sqube where ''p == q'').
//
// ''primeProof()'' evaluates a number whether it is prime-proof:
// - convert it to a string and modify every digit separately, be careful with the first digit because it must not be zero
// - run the Miller-Rabin primality test: if it returns ''true'', then the number is not prime-proof
//
// I added a few optimizations but they don't really improve performance:
// - don't run a prime a test on even numbers (last digit is even)
// - don't run a prime a test on the original number (every sqube is not prime)
//
// # Alternative
// My ''std::set'' is more or less a priority queue. It contains a few thousands elements (to be precise: 15888 when the 200th sqube is found).
// You can replace it by two nested loops iterating over ''p'' and ''q''. However, you need to choose reasonable limits for ''p'' and ''q'' and sort the result afterwards.

#include <iostream>
#include <string>
#include <vector>
#include <set>
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

// ---------- and now my solution ----------

// a sqube has value = p^2 * q^3
struct Sqube
{
  // note: this struct doesn't check whether p and q are different primes
  const unsigned int p;
  const unsigned int q;
  const unsigned long long value;

  // create a new sqube
  Sqube(unsigned int p_, unsigned int q_)
  : p(p_), q(q_), value((unsigned long long)p_*p_ * q_*q_*q_)
  {}

  // sort two squbes by their value, needed by std::set
  bool operator<(const Sqube& other) const
  {
    return value < other.value;
  }
};

// return true if changing a digit converts the number to a prime number
bool isPrimeProof(unsigned long long value)
{
  auto strValue = std::to_string(value);
  for (unsigned int pos = 0; pos < strValue.size(); pos++)
  {
    // an even number can only become prime when modifying the last digit
    if (value % 2 == 0)
      pos = strValue.size() - 1;

    // change digit by digit
    auto strModified = strValue;
    for (auto digit = '0'; digit <= '9'; digit++)
    {
      // no leading zero
      if (digit == '0' && pos == 0)
        continue;
      // last digit can't be even
      if (digit % 2 == 0 && pos == strValue.size() - 1) // ASCII codes of even digits are even, too
        digit++;                                        // strictly speaking this doesn't test 2 (which is a prime)
                                                        // but the next number 3 is prime and produced the correct result

      // no need to check the original value (a sqube is never prime)
      if (digit == strValue[pos])
        continue;

      // convert from string to binary
      strModified[pos] = digit;
      auto modified = std::stoull(strModified);

      // is it prime ?
      if (isPrime(modified))
        return false;
    }
  }

  return true;
}

int main()
{
  // count how many squbes contain "200"
  unsigned int sequence = 200;
  std::cin >> sequence;

  std::string  strSequence = std::to_string(sequence); // = "200"
  unsigned int count       = 0; // stop when count = 200

  // the two smallest squbes, my "seed values"
  std::set<Sqube> squbes = { Sqube(3, 2), Sqube(2, 3) };

  while (true) // abort/exit condition can be found inside the loop
  {
    // pick smallest sqube and remove it
    auto current = *(squbes.begin());
    squbes.erase(squbes.begin());

    // does it contain "200" ?
    auto strCurrent = std::to_string(current.value);
    if (strCurrent.find(strSequence) != std::string::npos &&
        isPrimeProof(current.value))
    {
      // yes, another match
      count++;

      // done ?
      if (count == sequence)
      {
        std::cout << strCurrent << std::endl;
        break;
      }
    }

    // add next squbes
    // find a sqube with the same q but p is the next prime (not equal to q)
    auto nextP = current.p + 1;
    while (nextP == current.q || !isPrime(nextP))
      nextP++;
    squbes.insert(Sqube(nextP, current.q));

    // find a sqube with the same p but q is the next prime (not equal to p)
    auto nextQ = current.q + 1;
    while (nextQ == current.p || !isPrime(nextQ))
      nextQ++;
    squbes.insert(Sqube(current.p, nextQ));
  }

  return 0;
}
