// ////////////////////////////////////////////////////////
// # Title
// Super Pandigital Numbers
//
// # URL
// https://projecteuler.net/problem=571
// http://euler.stephan-brumme.com/571/
//
// # Problem
// A positive number is pandigital in base `b` if it contains all digits from `0` to `b - 1` at least once when written in base `b`.
//
// A n-super-pandigital number is a number that is simultaneously pandigital in all bases from `2` to `n` inclusively.
// For example `978 = 1111010010_2 = 1100020_3 = 33102_4 = 12403_5` is the smallest 5-super-pandigital number.
// Similarly, 1093265784 is the smallest 10-super-pandigital number.
// The sum of the 10 smallest 10-super-pandigital numbers is 20319792309.
//
// What is the sum of the 10 smallest 12-super-pandigital numbers?
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// I made an assumption which turned out to be true:
// there are at least ten 12-super-pandigital numbers with 12 digits.
// Or in plain English: if I generate all permutations of the digits ''{ 0,1,2,3,4,5,6,7,8,9,10,11 }'' in lexicographical order
// and check whether they are 12-super-pandigital then I will find the result.
//
// The smallest valid 12-pandigital number is ''{ 1,0,2,3,4,5,6,7,8,9,10,11 }'' because leading zeros are forbidden.
// Any 12-pandigital number fits nicely in a 64 bit integer: `log_2 12^12 < 44` ==> I need at most 44 bits
// The smallest such number is `current = 1 * 12^11 + 0 * 12^10 + 2 * 12^9 + 3 * 12^8 + ... + 11 * 12^0 = 754777787027` (in decimal notation, base 10)
//
// I rewrote the function ''isPandigital()'' from problem 170 such that it accepts an arbitrary base greater than 1 and tolerates if a digit appears more than once.
// It basically chops off the right-most digit and sets a bit in a bitmask ''used''.
// If all 12 bits are set at the end, then the number is pandigital indeed.
//
// Since all numbers are already pandigital in base 12 I only have to check bases 2,3,...,11.
// It's much faster to check each number against a "high" base first because the "failure rate" is higher.
// For example, all numbers except zero and `2^n-1` are pandigital in base 2, so pretty much all numbers are pandigital in base 2.
// On the other side, a substantial amount of numbers fail the 11-pandigital test.
// Consequently, my ''for''-loop runs backwards from 11 to 2 and aborts if a ''isPandigital'' check fails.
//
// Division and modulo operations are notoriously slow on modern CPUs - and my program basically does that non-stop.
// Division and modulo operation can be quite fast if you divide by a power of two (and you have a smart compiler):
// Division by 8 is just a right shift by 3 bits (because `2^3 = 8`) and modulo 8 means taking the right-most three bits (''number & (8-1)'').
// Checking ''isPandigital(current, 8)'' before anything else made my program more than twice as fast !
//
// # Note
// The inner loop of ''isPandigital'' skips the last iteration because then ''number'' is a single digit and I can save one division and one modulo. This trick is about 5% faster.
//
// I was pretty sure that the ''for''-loop which calls ''isPandigital(current, base)'' can be optimized:
// if ''isPandigital(current, 8)'' was already tested outside the ''for''-loop then there is no use in tested against it inside the ''for''-loop.
// To my surprise, to opposite is true and I have no idea why.
//
// If a number passes the pandigital check for base=11,10,9,...,6 then it also passes the checks for base=5,4,3,2.

#include <iostream>
#include <vector>
#include <algorithm>

const unsigned int MaxBase = 12;

// return true if decimal number is pandigital in a certain base
// note: a digit may found more than once unlike most other pandigital problems
//       where each digit must be found exactly once
bool isPandigital(unsigned long long number, unsigned int base)
{
  // bitmask where the n-th bit is set if the digit n was observed in number
  unsigned int used = 0;
  // all bits set => all digits used
  const unsigned int All = (1 << base) - 1;

  // process right-most digit and remove it
  while (number >= base) // skip last iteration
  {
    auto digit = number % base;
    used |= 1 << digit;

    number /= base;
  }

  // simplified last iteration
  used |= 1 << number;

  return used == All;
}

int main()
{
  unsigned int base       = 12;
  unsigned int numResults = 10;
  std::cin >> base >> numResults;

  // smallest 12-pandigital number
  std::vector<unsigned char> twelve = { 1,0,2,3,4,5,6,7,8,9,10,11 };
  // reduce for smaller bases
  twelve.resize(base);

  // look for the sum of the first 10 matches
  unsigned int  numFound = 0;
  unsigned long long sum = 0;
  do
  {
    // convert from base 12 to an integer (technically that base-12-to-2 ?)
    unsigned long long current = 0;
    for (auto digit : twelve)
    {
      current *= base;
      current += digit;
    }

    // an optimizing compiler can reduce mod/div by fast bit operations
    if (base >= 8 && !isPandigital(current, 8))
      continue;

    // no need to check base 12 because all generated number are pandigital in base 12 by definition
    // I'm pretty sure there are some relationships:
    // - if pandigital in base 8 then always in base 4, too
    // - if pandigital in base 9 then always in base 3, too
    // my debugging output verified that no number is rejected by the tests in base 2,3,4,5
    bool isGood = true;
    for (auto i = base - 1; i >= 2; i--)
      if (!isPandigital(current, i)) // I tried to prepend "i != 8" but it was slower !
      {
        isGood = false;
        break;
      }

    // passed all tests
    if (isGood)
    {
      sum += current;
      numFound++;
      // done ?
      if (numFound == numResults)
        break;
    }
  } while (std::next_permutation(twelve.begin(), twelve.end()));

  // print result
  std::cout << sum << std::endl;
  return 0;
}
