// ////////////////////////////////////////////////////////
// # Title
// How many reversible numbers are there below one-billion?
//
// # URL
// https://projecteuler.net/problem=145
// http://euler.stephan-brumme.com/145/
//
// # Problem
// Some positive integers n have the property that the sum `n + reverse(n)` consists entirely of odd (decimal) digits.
// For instance, 36 + 63 = 99 and 409 + 904 = 1313. We will call such numbers reversible; so 36, 63, 409, and 904 are reversible.
// Leading zeroes are not allowed in either `n` or `reverse(n)`.
//
// There are 120 reversible numbers below one-thousand.
//
// How many reversible numbers are there below one-billion (`10^9`)?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// A simple and straightforward brute force solution - and it finishes in less than a second.
// I'm pretty sure there is a smarter approach but from time to time you need to unleash the raw power of modern CPUs ...
//
// Actually, I have added two important optimization:
// - there are no solutions between `10^8` and `10^9` ==> makes the program 10x faster
// - either the first or the last digit must be odd ==> assume the last is odd and multiply result by 2 ==> 2x faster

#include <iostream>
#include <vector>


// note: my code only reversed odd numbers, they can't produce a leading zero
unsigned int reverse(unsigned int x)
{
  unsigned int result = 0;
  while (x > 9) // speed optimization: skip the last digit
  {
    auto digit = x % 10;
    result *= 10;
    result += digit;
    x /= 10;
  }
  // simple processing of the last digit, saves one division/modulo ;-)
  return result * 10 + x;
}

// return true if all digits are odd
bool onlyOdd(unsigned int x)
{
  while (x > 0)
  {
    // found an even digit ?
    if (x % 2 == 0)
      return false;
    x /= 10;
  }
  // yes, all odd
  return true;
}

int main()
{
  // if the first digit is odd, then the last has to be even
  // the same logic applies in reverse order, too
  // therefore analyze only odd numbers and multiply the result by 2
  const unsigned int factor = 2;

  unsigned int count = 0;
  // no solutions between 10^8 and 10^9
  for (unsigned int i = 11; i < 100*1000*1000; i += factor)
    if (onlyOdd(i + reverse(i)))
      count += factor;

  std::cout << count << std::endl;
  return 0;
}

