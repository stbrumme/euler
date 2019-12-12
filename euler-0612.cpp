// ////////////////////////////////////////////////////////
// # Title
// Friend numbers
//
// # URL
// https://projecteuler.net/problem=612
// http://euler.stephan-brumme.com/612/
//
// # Problem
// Let's call two numbers friend numbers if their representation in base 10 has at least one common digit.
// E.g. 1123 and 3981 are friend numbers.
//
// Let `f(n)` be the number of pairs `(p,q)` with `1 <= p < q < n` such that `p` and `q` are friend numbers.
// `f(100)=1539`
//
// Find `f(10^18) mod 1000267129`.
//
// # Written by
// Stephan Brumme
// December 2017
//
// # Idea
// I came up with two brute force algorithms. Given enough time they could solve the problem - but you should be ready to wait a few years until you see the result ...
//
// Both algorithms rely on the function ''fingerprint'' with returns a bitmask: if its parameter ''x'' contains the digit ''n'' at least once, then the n-th bit of the result is set.
// For example ''fingerprint(1305)'' is ''00101011'' in binary (or 43 decimal) and ''fingerprint(52)'' is ''00100100'' in binary (= 36 in decimal).
// If two numbers are friend numbers then their fingerprints share at least one bit and ''(fingerprint(a) & fingerprint(b)) != 0''. Note: it's a single ''&'' because it's the binary AND, not the logical AND.
//
// Let's check whether 1305 and 52 are friends numbers:
// ''(fingerprint(1305) & fingerprint(52)) = 00101011b & 00100100b = 00100000b != 0'' ==> yes, they are friend numbers
//
// The first algorithm ''bruteForce'' is an implementation of the obvious `O(n^2)` strategy:
// compare each number ''q'' with all smaller numbers ''p''.
// It takes about 0.5 seconds to solve `f(10^4)` and 50 seconds to solve `f(10^5)`.
// Runtime for `f(10^n)` seems to be approximately `dfrac{10^{2n}}{2 * 10^8}` seconds. So `f(10^18)` needs `approx 5 * 10^27` seconds which is `10^10` longer than the universe existed. Oops.
//
// A significant speed-up can be achieved if switching from that basic `O(n^2)` algorithm to a slightly smarter `O(n)` algorithm:
// - there are 10 different digits in the decimal system, so we have at most `2^10` different bitmasks (actually it's `2^10 - 1` because every numbers has at least one digit but let's ignore that)
// - I need to keep track of `2^10` running counters: how often each bitmask was found so far
// - for each number: add all counters whose associated bitmask ''i'' is "compatible" to the current numbers bitmask
// - "compatible" means that a counter's bitmask ''i'' shares at least one bit with ''fingerprint(current)''
//
// It takes one second to solve `f(10^6)`, ten seconds for `f(10^7)` and about 103 seconds for `f(10^8)`, a nice linear behaviour.
// Runtime for `f(10^n)` seems to be approximately `10^n * 10^-6` seconds. So `f(10^18)` needs `approx 10^12` seconds which is close to 110000 years.
//
// It's getting late now and I'm too sleepy to figure out a proper way to solve that problem ...

#include <iostream>

// taken from problem statement
const unsigned int Modulo = 1000267129;

// set the n-th bit if x contains digit n
unsigned int fingerprint(unsigned long long x)
{
  unsigned int result = 0;
  while (x > 0)
  {
    auto digit = x % 10;
    result |= 1 << digit;
    x /= 10;
  }
  return result;
}

// O(n^2) algorithm, compare each number q with all smaller numbers p
unsigned long long bruteForce(unsigned long long limit)
{
  unsigned long long result = 0;
  for (unsigned long long q = 1; q < limit; q++)
  {
    // bitmask of used digits
    auto maskQ = fingerprint(q);

    for (unsigned long long p = 1; p < q; p++)
    {
      auto maskP = fingerprint(p);
      // share at least one digit
      if ((maskQ & maskP) != 0)
      {
        result++;
        // much faster than %-operator
        if (result == Modulo)
          result = 0;
      }
    }
  }

  return result;
}

// O(n) algorithm, much faster than bruteForce() but still too slow to handle limit = 10^18
// but still useful up to 10^8
unsigned long long slow(unsigned long long limit)
{
  unsigned long long result = 0;

  // setup counters for each bitmask (10 digits => 2^10 bitmasks
  const auto NumCounters = 1 << 10;
  unsigned long long maskCount[NumCounters] = { 0 };

  // iterate over all numbers
  for (unsigned long long q = 1; q < limit; q++)
  {
    // get current bitmask
    auto maskQ = fingerprint(q);

    // add all counters that share at least one bit with the current bitmask
    for (auto i = 1; i < NumCounters; i++)
      if (i & maskQ)
      {
        result += maskCount[i];
        if (result >= Modulo)
          result %= Modulo;
      }

    // add current number to its bitmask counter
    maskCount[maskQ]++;
    // avoid overflows
    if (maskCount[maskQ] == Modulo)
      maskCount[maskQ] = 0;
  }

  return result;
}

int main()
{
  // friends(    100) =             1539
  // friends(   1000) =           289665
  // friends(  10000) =         39235977
  // friends( 100000) =       4528635021 % 1000267129 = 527566505
  // friends(1000000) =     481858831665 % 1000267129 = 730342616
  // friends(   10^7) =   49369806187101 % 1000267129 = 621768177
  // friends(   10^8) = 4979780329300065 % 1000267129 = 440930015
  unsigned long long limit = 1000000ULL;
  std::cin >> limit;
  //std::cout << bruteForce(limit) << std::endl;
  std::cout << slow(limit) << std::endl;
  return 0;
}
