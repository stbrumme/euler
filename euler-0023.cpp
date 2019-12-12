// ////////////////////////////////////////////////////////
// # Title
// Non-abundant sums
//
// # URL
// https://projecteuler.net/problem=23
// http://euler.stephan-brumme.com/23/
//
// # Problem
// A perfect number is a number for which the sum of its proper divisors is exactly equal to the number.
// For example, the sum of the proper divisors of 28 would be 1 + 2 + 4 + 7 + 14 = 28, which means that 28 is a perfect number.
//
// A number n is called deficient if the sum of its proper divisors is less than n and it is called abundant if this sum exceeds n.
//
// As 12 is the smallest abundant number, 1 + 2 + 3 + 4 + 6 = 16, the smallest number that can be written as the sum of two abundant numbers is 24.
// By mathematical analysis, it can be shown that all integers greater than 28123 can be written as the sum of two abundant numbers.
// However, this upper limit cannot be reduced any further by analysis even though it is known that the greatest number that cannot be expressed as the sum of two abundant numbers is less than this limit.
//
// Find the sum of all the positive integers which cannot be written as the sum of two abundant numbers.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The function ''getSum'' was taken from problem 21.
// It can be used to find all abundant numbers below 28124 (see problem statement) which I store in ''abundant''.
//
// The function ''isAbundantSum'' returns true if it finds at least one combination of two abundant numbers.
// For each element ''i'' of ''abundant'' it computes ''other = x - i''. If ''other'' is an element of ''abundant'', then it return ''true''.
//
// # Note
// The "live test" is based on the Hackerrank problem.
//
// # Hackerrank
// Again, the original problem was heavily modified by Hackerrank:
// instead of asking for the sum it just asks to figure out for a single number whether it can be written as the sum
// of two abundant numbers or not and print ''YES'' or ''NO'' accordingly.

#include <iostream>
#include <set>

// constant according to problem statement
const unsigned int EverythingsASumFromHere = 28124;

// will contain all abundant numbers
std::set<unsigned int> abundant;

// generate sum of all divisor's of x
unsigned int getSum(unsigned int x)
{
  // note: code very similar to problem 21

  // find all factors:
  // look only for the "smaller" divisors <= sqrt(x)
  // and they have a "bigger" brother x / divisor

  // 1 is always a divisor, but not the number x itself
  unsigned int divisorSum = 1;
  // therefore start at 2
  for (unsigned int divisor = 2; divisor * divisor <= x; divisor++)
    if (x % divisor == 0)
    {
      divisorSum += divisor;

      // add the "bigger brother"
      unsigned int otherDivisor = x / divisor;
      // except square numbers
      if (otherDivisor != divisor)
        divisorSum += otherDivisor;
    }

  return divisorSum;
}

// return true if parameter can be written as the sum of two abundant numbers
bool isAbundantSum(unsigned int x)
{
  // big numbers are always an abundant sum
  if (x >= EverythingsASumFromHere)
    return true;

  // look at all small abundant numbers in ascending order
  for (auto i : abundant)
  {
    // abort if there is no sum possible
    if (i >= x) // or even faster: if (i > x/2)
      return false;

    // is its partner abundant, too ?
    unsigned int other = x - i;
    if (abundant.count(other) == 0)
      continue;

    // yes, we found a valid combination
    return true;
  }

  // nope
  return false;
}

int main()
{
  // precomputation step:
  // find all abundant numbers <= 28123
  for (unsigned int i = 1; i < EverythingsASumFromHere; i++) // actually, we could start at 12
    // divisor's sum bigger than the number itself ?
    if (getSum(i) > i)
      abundant.insert(i);

//#define ORIGINAL
#ifdef ORIGINAL
  unsigned long long sum = 0;
  for (unsigned int i = 0; i < EverythingsASumFromHere; i++)
  {
    // sum of all numbers which cannot be written as an abundant sum
    if (!isAbundantSum(i))
      sum += i;
  }
  std::cout << sum << std::endl;

#else

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // find out whether a certain number can be written as an abundant sum
    unsigned int x;
    std::cin >> x;
    std::cout << (isAbundantSum(x) ? "YES" : "NO") << std::endl;
  }
#endif
  return 0;
}
