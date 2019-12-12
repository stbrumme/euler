// ////////////////////////////////////////////////////////
// # Title
// Step Numbers
//
// # URL
// https://projecteuler.net/problem=178
// http://euler.stephan-brumme.com/178/
//
// # Problem
// Consider the number 45656.
// It can be seen that each pair of consecutive digits of 45656 has a difference of one.
// A number for which every pair of consecutive digits has a difference of one is called a step number.
// A pandigital number contains every decimal digit from 0 to 9 at least once.
// How many pandigital step numbers less than `10^40` are there?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// And again: it's time for Dynamic Programming ...
// My ''search'' function has three parameters:
// 1. a mask where bit x is set if any of the digits in front of the current digit was x.
// 2. the current digit
// 3. how many digits still have to be processed
//
// A number is pandigital if the all bits 0..9 are set (''AllDigits = (1 << 10) - 1'' which is 1023).
// If the current digit is bigger than 0, then the next can be "a step down".
// If the current digit is smaller than 9, then the next can be "a step up".
//
// Of course, this algorithm would be far too slow, but the algorithm can heavily memoize results.
// The function's parameters are merged into a single unique ID and stored in a ''cache'' (with 1024*40*10 = 409600 entries, which requires about 3 MByte RAM).
//
// # Note
// If the first digit is between 1 and 4 (inclusive), then for every solution there is a "mirrored" version, where each digit `b_i = 9 - a_i`.
// For example, 10123456789 is a solution and therefore 89876543210 is a solution, too.
// How does it help ? Well, you can compute all solutions starting with 1..4, multiply that number by 2 and add all solutions starting with 9.
//
// # Hackerrank
// They want you to compute the result for `n < 10^{10^4}`. I can't figure out whether their input is always a power-of-ten or not.
// The result will easily be too large for C++'s 64 bit integers anyway.

#include <iostream>
#include <vector>

// initial state: no digits used, all bits are zero
const unsigned int NoDigits  =  0;
// all ten digits used => all ten bits set
const unsigned int AllDigits = (1 << 10) - 1; // = 1023
// analyze all numbers with up to 10^40
const unsigned int MaxDigits = 40;

// count pandigital step number where bits in "mask" are set according to the first digits
// and the current digit is "currentDigit" with "numDigitsLeft" digits to go
unsigned long long search(unsigned int mask, unsigned int currentDigit, unsigned int numDigitsLeft)
{
  // update mask, "use digit"
  mask |= 1 << currentDigit;

  // arrived at last digit ?
  if (numDigitsLeft == 1)
  {
    // all ten bits set ? => pandigital
    if (mask == AllDigits)
      return 1; // yes, a solution

    // not pandigital, no valid solution
    return 0;
  }

  // memoize
  unsigned int hash = mask * MaxDigits * 10 + (numDigitsLeft - 1) * 10 + currentDigit;
  static std::vector<unsigned long long> cache(1024 * 10 * MaxDigits, 0);
  // result already known ?
  if (cache[hash] != 0)
    return cache[hash];

  unsigned long long result = 0;
  // next digit is smaller
  if (currentDigit > 0)
    result += search(mask, currentDigit - 1, numDigitsLeft - 1);
  // next digit is bigger
  if (currentDigit < 9)
    result += search(mask, currentDigit + 1, numDigitsLeft - 1);

  cache[hash] = result;
  return result;
}

int main()
{
  // maxDigits is the actual number of digits
  // MaxDigits is the highest allowed value for maxDigits
  // I know, it's confusing ...
  unsigned int maxDigits = 40;
  std::cin >> maxDigits;
  // note: input syntax differs from Hackerrank !

  unsigned long long result = 0;
  // for each number of digits ...
  for (unsigned int numDigits = 1; numDigits <= maxDigits; numDigits++)
  {
    // ... the first one must be 1..9 (and never zero !)
    for (unsigned int digit = 1; digit <= 9; digit++)
      result += search(NoDigits, digit, numDigits);

    // a little bit faster:
    //result += 2 * search(NoDigits, 1, numDigits);
    //result += 2 * search(NoDigits, 2, numDigits);
    //result += 2 * search(NoDigits, 3, numDigits);
    //result += 2 * search(NoDigits, 4, numDigits);
    //result +=     search(NoDigits, 9, numDigits);
  }

  std::cout << result << std::endl;
  return 0;
}
