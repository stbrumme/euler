// ////////////////////////////////////////////////////////
// # Title
// Circular primes
//
// # URL
// https://projecteuler.net/problem=35
// http://euler.stephan-brumme.com/35/
//
// # Problem
// The number, 197, is called a circular prime because all rotations of the digits: 197, 971, and 719, are themselves prime.
//
// There are thirteen such primes below 100: 2, 3, 5, 7, 11, 13, 17, 31, 37, 71, 73, 79, and 97.
//
// How many circular primes are there below one million?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// First, a a standard prime sieve finds all prime numbers up to our limit (1000000 by default) and keeps them in a ''std::set''.
//
// Then each prime ''x'' in ''std::set'' is rotated by one digit to the right:
// 1. get the right-most digit:
// ''auto digit = rotated % 10;''
// 2. move all digits by one digit to the right ("erasing" the right-most digit):
// ''rotated /= 10;''
// 3. prepend the right-most digit:
// '' rotated += digit * shift;''
// 4. check whether rotated is part of our ''std::set'', too
// 5. if ''rotated'' is equal to our initial value ''x'' then we checked all rotations
//
// The only point of interest is ''shift'' which is a power of 10 such that `10^a = shift <= x <= 10^{a+1}`.
// E.g., if `x = 3456` then `shift = 1000`.
//
// # Note
//
// There are a few options to speed up the code:
// 1. All prime numbers are odd (except for 2): if ''x != 2'' and any digit is even then this prime can't be circular.
// 2. We can simplify point 1 by noting that all single-digit primes are circular.
//
// # Hackerrank
// We have to find the sum of all such prime numbers, not their count.

#include <iostream>
#include <set>

int main()
{
  // highest number (1000000 in original problem)
  unsigned int n;
  std::cin >> n;

  // precomputation: find all prime numbers up to n
  std::set<unsigned int> primes;
  primes.insert(2);
  for (unsigned int i = 3; i <= n; i += 2)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto x : primes)
    {
      // divisible => not prime
      if (i % x == 0)
      {
        isPrime = false;
        break;
      }

      // prime is too large to be a divisor
      if (x*x > i)
        break;
    }

    // yes, we have a prime
    if (isPrime)
      primes.insert(i);
  }

  // now look at all primes
  unsigned int sum = 0;
  for (auto x : primes)
  {
    // move the right-most digit to the front of the number
    // we need to know the "position" of the front-most digit:
    // shift will be   1 for x =   1..9
    // shift will be  10 for x =  10..99
    // shift will be 100 for x = 100..999 and so on
    unsigned int shift = 1;
    while (x > shift * 10)
      shift *= 10;

    auto rotated = x;
    do
    {
      // take right-most digit
      auto digit = rotated % 10;
      // remove it
      rotated /= 10;
      // and prepend it
      rotated += digit * shift;

      // rotated number not prime ?
      if (primes.count(rotated) == 0)
        break;
    } while (rotated != x); // finished the circle ? (we have the initial number again)

    // all rotations succeeded ?
#define ORIGINAL
#ifdef ORIGINAL
    if (rotated == x)
      sum++;
#else
    if (rotated == x)
      sum += x;
#endif
  }

  std::cout << sum << std::endl;
  return 0;
}
