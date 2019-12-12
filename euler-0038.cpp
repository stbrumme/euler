// ////////////////////////////////////////////////////////
// # Title
// Pandigital multiples
//
// # URL
// https://projecteuler.net/problem=38
// http://euler.stephan-brumme.com/38/
//
// # Problem
// Take the number 192 and multiply it by each of 1, 2, and 3:
//
// `192 * 1 = 192`
// `192 * 2 = 384`
// `192 * 3 = 576`
//
// By concatenating each product we get the 1 to 9 pandigital, 192384576. We will call 192384576 the concatenated product of 192 and (1,2,3)
// The same can be achieved by starting with 9 and multiplying by 1, 2, 3, 4, and 5, giving the pandigital, 918273645, which is the concatenated product of 9 and (1,2,3,4,5).
//
// What is the largest 1 to 9 pandigital 9-digit number that can be formed as the concatenated product of an integer with (1,2, ... , n) where n > 1?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// A bitmask tracks whether a number is pandigital (or not): if the n-th bit of ''bitsUsed'' is 1 then n is a digit of ''pandigital''.
// A truly pandigital number has a bitmask of ''bitsAll = (1 << 9) | (1 << 8) | ... | (1 << 2) | (1 << 1)''.
// According to the problem statement, a digit must not be zero. That means the lowest bit is always 0:
// `bitsAll_9=1022` (and `bitsAll_8=510` for alternate Hackerrank input).
//
// My program process all numbers ''i'' from 2 to ''maxFactor'' and multiplies them with 1, 2, ... until the concatenated result's ''bitsUsed == bitsAll''.
//
// Each ''product = i * multiplier'' (where ''multiplier = 1, 2, ...'') is split into its digits and each digit added to the bitmask ''bitsUsed''.
// When a collision occurs (that digit was already used before), then ''bitsUsed'' is set to an invalid value and the next number ''i'' can be processed.
//
// # Note
// ''maxFactor'' cannot exceed 10000 because then ''i*1''+''i*2'' will have more than 9 digits.
// A few things are probably much easier when converting all numbers to ''std::string'' - at the cost of speed.
//
// # Hackerrank
// The result may have either 8 or 9 digits and there is an upper limit for the start value.

#include <iostream>

int main()
{
  unsigned int maxFactor, maxDigit;  // 10000 and 9 for the original problem
  std::cin >> maxFactor >> maxDigit;

  // bitmask if all digits are used
  unsigned int bitsAll = 0;
  for (unsigned int i = 1; i <= maxDigit; i++)
    bitsAll |= 1 << i;

#define ORIGINAL
#ifdef ORIGINAL
  // largest pandigital number found so far
  unsigned int largest = 0;
#endif

  // try all numbers
  for (unsigned int i = 2; i <= maxFactor; i++)
  {
    // the whole pandigital number
    unsigned int pandigital = 0;

    // multiply i by 1,2,3,...
    unsigned int multiplier = 1;

    // bitmask of used digits (nth bit is set if pandigital contains digit n)
    unsigned int bitsUsed   = 0;

    while (bitsUsed < bitsAll)
    {
      // next step
      unsigned int product = i * multiplier;

      // extract right-most digit
      while (product > 0)
      {
        // extract right-most digit
        unsigned int digit = product % 10;
        // remove it
        product    /= 10;

        // make room to add i*multiplier lateron
        pandigital *= 10;

        // remember all digits we used
        unsigned int bitMask = 1 << digit;
        // we already had this digit ?
        if (digit == 0 || (bitsUsed & bitMask) != 0)
        {
          bitsUsed = bitsAll + 1; // set to an invalid value
          break;
        }

        // mark current digit as "used"
        bitsUsed |= bitMask;
      }

      // keep going in the sequence
      pandigital += i * multiplier;
      multiplier++;
    }

    // enough digits generated ?
    if (bitsUsed == bitsAll)
    {
#ifdef ORIGINAL
      if (largest < pandigital)
        largest = pandigital;
#else
      std::cout << i << std::endl;
#endif
    }
  }

#ifdef ORIGINAL
  std::cout << largest << std::endl;
#endif

  return 0;
}
