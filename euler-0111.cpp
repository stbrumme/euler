// ////////////////////////////////////////////////////////
// # Title
// Primes with runs
//
// # URL
// https://projecteuler.net/problem=111
// http://euler.stephan-brumme.com/111/
//
// # Problem
// Considering 4-digit primes containing repeated digits it is clear that they cannot all be the same:
// 1111 is divisible by 11, 2222 is divisible by 22, and so on.
// But there are nine 4-digit primes containing three ones:
// 1117, 1151, 1171, 1181, 1511, 1811, 2111, 4111, 8111
//
// We shall say that M(n, d) represents the maximum number of repeated digits for an n-digit prime where d is the repeated digit,
// N(n, d) represents the number of such primes, and S(n, d) represents the sum of these primes.
//
// So M(4, 1) = 3 is the maximum number of repeated digits for a 4-digit prime where one is the repeated digit,
// there are N(4, 1) = 9 such primes, and the sum of these primes is S(4, 1) = 22275.
// It turns out that for d = 0, it is only possible to have M(4, 0) = 2 repeated digits, but there are N(4, 0) = 13 such cases.
//
//In the same way we obtain the following results for 4-digit primes.
//
// ||    5    ||    5    ||    5    ||    5    ||
// || Digit d ++  M(4,d) ++  N(4,d) ++  S(4,d) ||!
// ||    0    ++    2    ++   13    ++   67061 ||
// ||    1    ++    3    ++    9    ++   22275 ||
// ||    2    ++    3    ++    1    ++    2221 ||
// ||    3    ++    3    ++   12    ++   46214 ||
// ||    4    ++    3    ++    2    ++    8888 ||
// ||    5    ++    3    ++    1    ++    5557 ||
// ||    6    ++    3    ++    1    ++    6661 ||
// ||    7    ++    3    ++    9    ++   57863 ||
// ||    8    ++    3    ++    1    ++    8887 ||
// ||    9    ++    3    ++    7    ++   48073 ||
//
// For d = 0 to 9, the sum of all S(4, d) is 273700.
//
// Find the sum of all S(10, d).
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// ''search'' iterates through all numbers and calls ''isPrime'' to figure out whether it's a prime number.
// There are a few gotchas:
// 1. While iterating, I create all possible strings. They may contain leading zeros, which are not allowed.
// 2. Add numbers into the sequence of repeated digits only if the number's digits are in non-decreasing order: 125 is okay, 122 is okay, 121 isn't.
//
// # Hackerrank
// I should analyze numbers with up to 40 digits (instead of 10). My code is too slow to handle anything with more than 12 digits.
// Even worse, my routines can't process numbers exceeding 64 bits (which is about 19 digits).

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define ORIGINAL

// return true if x is prime, based on trial division (not suitable for large numbers)
bool isPrime(unsigned long long x)
{
  // trial divison by 2
  if (x % 2 == 0)
    return x == 2;

  // trial division by odd numbers
  for (unsigned long long i = 3; i*i <= x; i++)
    if (x % i == 0)
      return false;

  return true;
}

// find all numbers where "digit" is repeated "repeat" times and "extraDigits" digits are added
// e.g. 10007 is found by search(0, 3, 2)
unsigned long long search(unsigned int digit, unsigned int repeat, unsigned int extraDigits, bool printPrimes = false)
{
  // sum of all matching primes
  unsigned long long sum = 0;
  std::vector<unsigned long long> matches; // Hackerrank only: print matches in ascending order

  // create string with a digit repeated a few times
  std::string sameDigit(repeat, digit + '0');

  // insert numbers i where 0 <= i < maxExtra
  // e.g. if extraDigits = 3 then from 0 to 999 (actually from "000" to "999")
  unsigned long long maxExtra = 1;
  for (unsigned int i = 1; i <= extraDigits; i++)
    maxExtra *= 10;

  // let's insert all those numbers !
  for (unsigned long long extra = 0; extra < maxExtra; extra++)
  {
    // convert to string
    auto current = std::to_string(extra);
    // is in ascending order ?
    auto sorted = current;
    std::sort(sorted.begin(), sorted.end());
    if (current != sorted)
      continue;

    // add zeros if "extra" has too few digits
    while (current.size() < extraDigits)
      current = '0' + current;

    // and add the bunch of identical digits
    current += sameDigit;
    // sort everything to ensure that std::next_permutation works properly
    std::sort(current.begin(), current.end());

    do
    {
      // no leading zeros
      if (current.front() == '0')
        continue;
      // no even numbers
      if (current.back() % 2 == 0) // ASCII code of even digits is even, too
        continue;

      // is it a prime ?
      unsigned long long num = std::stoll(current);
      if (isPrime(num))
      {
        sum += num;
        if (printPrimes)
          matches.push_back(num);
      }
    } while (std::next_permutation(current.begin(), current.end()));
  }

  // Hackerrank only: print all primes we found
  if (printPrimes && !matches.empty())
  {
    std::sort(matches.begin(), matches.end());
    for (auto x : matches)
      std::cout << x << " ";
  }

  return sum;
}


int main()
{
#ifdef ORIGINAL
  // total number of digits
  unsigned int digits = 10;
  std::cin >> digits;
  // sum of all matching primes
  unsigned long long sum = 0;

  // iterate over all repeated digits
  for (unsigned int i = 0; i <= 9; i++)
  {
    // try to use as many repeated digits as possible
    for (unsigned int repeated = digits - 1; repeated >= 1; repeated--)
    {
      auto insertDigits = digits - repeated;
      auto found = search(i, repeated, insertDigits);
      // found at least one prime ?
      if (found > 0)
      {
        sum += found;
        break;
      }
    }
  }

  std::cout << sum << std::endl;

#else

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int digits;
    unsigned int sameDigit;
    std::cin >> digits >> sameDigit;

    if (digits < 19) // my code can't handle > 64 bits at the moment
    {
      // iterate over all repeated digits
      for (unsigned int repeated = digits - 1; repeated >= 1; repeated--)
      {
        auto insertDigits = digits - repeated;
        if (search(sameDigit, repeated, insertDigits, true) > 0)
          break;
      }
    }

    std::cout << std::endl;
  }
#endif

  return 0;
}
