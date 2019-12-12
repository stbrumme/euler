// ////////////////////////////////////////////////////////
// # Title
// Digit factorials
//
// # URL
// https://projecteuler.net/problem=34
// http://euler.stephan-brumme.com/34/
//
// # Problem
// `145` is a curious number, as `1! + 4! + 5! = 1 + 24 + 120 = 145`.
//
// Find the sum of all numbers which are equal to the sum of the factorial of their digits.
//
// __Note:__ as `1! = 1` and `2! = 2` are not sums they are not included.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// This problem is very similar to problem 30.
//
// There is no 8-digit number which can be the sum of the factorials of its digits because `8 * 9! = 2903040` is a 7-digit number.
//
// I precomputed the factorials `0!` to `9!` instead of writing a short and simple __factorial__ function.
// Each number is split into its digits (again I begin with the least-significant, "I chop them from the right side")
// and then the factorial of these digits is looked up and added.
//
// Nothing spectacular - a very easy problem.
//
// # Hackerrank
// The sums must be divisible by the number, not equal.

#include <iostream>

int main()
{
  // precompute factorials of all possible digits 0!..9!
  const unsigned int factorials[] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 };

  // no more than 7*9! = 2540160 for the original problem
  unsigned int limit;
  std::cin >> limit;

  // result (differs for Hackerrank modified problem !)
  unsigned int result = 0;

  for (unsigned int i = 10; i < limit; i++)
  {
    unsigned int sum = 0;

    // split i into its digits
    unsigned int x = i;
    while (x > 0)
    {
      // add factorial of the right-most digit
      sum += factorials[x % 10];
      // remove that digit
      x /= 10;
    }

#define ORIGINAL
#ifdef ORIGINAL
    // equal ?
    if (sum == i)
      result += i;
#else
    // divisible ?
    if (sum % i == 0)
      result += i;
#endif
  }

  std::cout << result << std::endl;
  return 0;
}
