// ////////////////////////////////////////////////////////
// # Title
// Truncatable primes
//
// # URL
// https://projecteuler.net/problem=37
// http://euler.stephan-brumme.com/37/
//
// # Problem
// The number 3797 has an interesting property. Being prime itself, it is possible to continuously remove digits from left to right,
// and remain prime at each stage: 3797, 797, 97, and 7. Similarly we can work from right to left: 3797, 379, 37, and 3.
//
// Find the sum of the only eleven primes that are both truncatable from left to right and right to left.
//
// __NOTE:__ 2, 3, 5, and 7 are not considered to be truncatable primes.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// A generic prime sieve is mixed with my check for "prime-truncatability": as soon as a prime number is found, I remove digits step-by-step from the right side
// and make sure that the truncated number is still a prime number. In pseudo-code:
//
// ''while (number > 0 && primes.count(number) != 0)''
// ''  remove_one_digit(number)'' <== see explanation below
//
// If ''number == 0'' at the end of the ''while''-loop then ''number'' is truncatable and prime.
//
// Removing the right-most digit is simple: just divide by 10 and ignore any remainder. See the code that processes ''right''.
// Removing the left-most digit takes a little more effort (look out for ''left'' and ''factor''):
// Find that largest power-of-10 that is still smaller than the current number (that's my ''factor'').
// Then the remainder of ''left % factor'' chops off the left-most digit.
//
// # Hackerrank
// Hackerrank gave indirectly away that all such numbers are less than 1000000 (which is confirmed by OEIS A020994).
// Hackerrank's problem asks for a user-defined upper limit.

#include <set>
#include <iostream>

int main()
{
  // find all primes up to
  unsigned int n;
  std::cin >> n; // 1000000 is sufficient for the original problem

  // will contain all primes found so far
  std::set<unsigned int> primes;
  // all single-digit prime numbers (2,3,5,7) are not truncatable by definition
  primes.insert(2);
  primes.insert(3);
  primes.insert(5);
  primes.insert(7);

  unsigned int sum = 0;
  // check prime numbers with at least two digits
  // note: even numbers cannot be prime (except 2)
  for (unsigned int i = 11; i < n; i += 2)
  {
    bool isPrime = true;
    // check against all known primes
    for (auto p : primes)
    {
      // no more prime factors possible
      if (p*p > i)
        break;

      // divisible by another prime ? => i is not prime
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    if (!isPrime)
      continue;

    // now we have a prime
    primes.insert(i);

    // check whether truncable from the right side
    auto right = i;
    // remove right-most digit if still prime and until no digits remain
    while (right > 0 && primes.count(right) != 0)
      right /= 10;
    // pass only if all digits were successfully removed
    if (right != 0)
      continue;

    // same idea from the left side
    auto left = i;
    // find position of left-most digit
    unsigned int factor = 1;
    while (factor * 10 <= left)
      factor *= 10;
    // remove left-most digit if still prime and until no digits remain
    while (left > 0 && primes.count(left) != 0)
    {
      // fast version:
      left %= factor;
      // slower version: subtract until highest digit is completely gone (=zero)
      //while (left >= factor)
      // left -= factor;

      // okay, next digit is 10 times smaller
      factor /= 10;
    }
    // pass only if all digits were successfully removed
    if (left != 0)
      continue;

    // yeah, passed all tests !
    sum += i;
  }

  std::cout << sum << std::endl;
  return 0;
}
