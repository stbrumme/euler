// ////////////////////////////////////////////////////////
// # Title
// Digit factorial chains
//
// # URL
// https://projecteuler.net/problem=74
// http://euler.stephan-brumme.com/74/
//
// # Problem
// The number 145 is well known for the property that the sum of the factorial of its digits is equal to 145:
// 1! + 4! + 5! = 1 + 24 + 120 = 145
//
// Perhaps less well known is 169, in that it produces the longest chain of numbers that link back to 169; it turns out that there are only three such loops that exist:
//
// 169 ==> 363601 ==> 1454 ==> 169
// 871 ==> 45361 ==> 871
// 872 ==> 45362 ==> 872
//
// It is not difficult to prove that EVERY starting number will eventually get stuck in a loop. For example,
//
// 69 ==> 363600 ==> 1454 ==> 169 ==> 363601 (==> 1454)
// 78 ==> 45360 ==> 871 ==> 45361 (==> 871)
// 540 ==> 145 (==> 145)
//
// Starting with 69 produces a chain of five non-repeating terms, but the longest non-repeating chain with a starting number below one million is sixty terms.
//
// How many chains, with a starting number below one million, contain exactly sixty non-repeating terms?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// A function named ''fingerprintFactorial'' counts the number of digits. It's very similar to ''fingerprint'' from problem 49, problem 52 and problem 62.
// Numbers with the same fingerprint are permutations of each other. The sum of the factorials of their digits will be the same.
// This allows for a minor tweak: 0! = 1! = 1. Therefore ''fingerprintFactorial'' treats each 1 as a 0.
//
// Whenever the length of a loop has to be determined, the ''std::vector'' named ''loop'' is filled with the factorial sums until we encounter one for the second time.
//
// ''cache'' remembers all loop lengths indexed by their starting number. That cache is important to speed up Hackerrank repeated test cases.
//
// Some numbers "don't fit" into my scheme: those linking back to themselves don't report the correct loop length (they are off-by-one).
// That's why I manually adjust the result for those. All such "execeptional numbers" can be found in the problem statement:
// 145, 169, 871, 872, 1454, 45361, 45362 and 363601
//
// # Hackerrank
// I have to print the starting number of such loops (not just count them).

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

// count digits, two numbers have the same fingerprint if they are permutations of each other
unsigned long long fingerprintFactorial(unsigned int x)
{
  unsigned long long result = 0;
  while (x > 0)
  {
    unsigned int digit = x % 10;
    x /= 10;

    // small optimization: 0! = 1!
    if (digit == 1)
      digit = 0;

    // find the right location of the digit's counter
    unsigned long long shift = 1;
    for (unsigned int i = 0; i < digit; i++)
      shift *= 10;

    result += shift;
  }
  return result;
}

int main()
{
  // known loop-sizes
  // [first number] => [loop size]
  std::map<unsigned long long, unsigned int> cache;

  // special treatment for those numbers that link back to themselves
  // and are encountered before any other number of the same fingerprint
  cache[fingerprintFactorial(145)] = 1 + 1;
  cache[fingerprintFactorial(169)] = 3 + 1;

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int limit = 1000000;
    unsigned int loopLength = 60;
    std::cin >> limit >> loopLength;

    // count numbers with a certain loop length
    unsigned int result = 0;

    for (unsigned int i = 0; i <= limit; i++)
    {
      // determine loop length only for unknown fingerprints
      unsigned long long id = fingerprintFactorial(i);
      if (cache.count(id) == 0)
      {
        // all numbers of the current loop
        std::vector<unsigned int> loop;

        // add current number to loop, abort if insert fails (because element already exists)
        unsigned int x = i;
        while (std::find(loop.begin(), loop.end(), x) == loop.end() && loop.size() <= loopLength)
        {
          loop.push_back(x);

          // compute factorial sum of digits
          unsigned int facSum = 0;
          do
          {
            // factorials 0! ... 9!
            const unsigned int fac[10] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 };

            // add factorial of right-most digit
            facSum += fac[x % 10];
            // shift all digits to the right by one
            x /= 10;
          } while (x > 0);
          x = facSum;
        }

        // remember that loop length (for all upcoming permutations)
        cache[id] = loop.size();
      }

      // now the loop length is available at cache[id]
      // correct loop length ?
      bool match = (cache[id] == loopLength);

      // special treatment for those numbers that link back to themselves
      // (false negatives and positives) => relevant only for Hackerrank
      if (i == 145)
        match = (loopLength == 1);
      if (i == 169 || i == 1454 || i == 363601)
        match = (loopLength == 3);
      if (i == 871 || i == 872 || i == 45361 || i == 45362)
        match = (loopLength == 2);

      // count matches
      if (match)
        result++;

//#define ORIGINAL
#ifndef ORIGINAL
      if (match)
        std::cout << i << " ";
#endif
    }

#ifdef ORIGINAL
    std::cout << result << std::endl;
#else
    if (result == 0)
      std::cout << "-1";
    std::cout << std::endl;
#endif
  }
}
