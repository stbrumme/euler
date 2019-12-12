// ////////////////////////////////////////////////////////
// # Title
// Digit fifth powers
//
// # URL
// https://projecteuler.net/problem=30
// http://euler.stephan-brumme.com/30/
//
// # Problem
// Surprisingly there are only three numbers that can be written as the sum of fourth powers of their digits:
//
// `1634 = 1^4 + 6^4 + 3^4 + 4^4`
// `8208 = 8^4 + 2^4 + 0^4 + 8^4`
// `9474 = 9^4 + 4^4 + 7^4 + 4^4`
//
// As `1 = 1^4` is not a sum it is not included.
//
// The sum of these numbers is 1634 + 8208 + 9474 = 19316.
//
// Find the sum of all the numbers that can be written as the sum of fifth powers of their digits.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The sum of the digits' fifth powers is maximized when each digit is 9:
// 1 digit:  `1 * 9^5 = 59049`
// 2 digits: `2 * 9^5 = 118098`
// 3 digits: `3 * 9^5 = 177147`
// 4 digits: `4 * 9^5 = 236196`
// 5 digits: `5 * 9^5 = 295245`
// 6 digits: `6 * 9^5 = 354294`
// 7 digits: `7 * 9^5 = 413343`
//
// The last line is pretty interesting: it's impossible for a seven-digit number to have a seven-digit sum of its digits' fifth powers,
// because all those sums would have at most six digits.
//
// If we analyse all numbers from 2 to 354294 (maximum sum for 6 digits) then we can solve the problem:
// 1. split each number into its digits
// 2. add all fifth powers of these digits
// 3. if the sum is equal to the original number then add it to our result
//
// # Hackerrank
// The exponent varies between 3 and 6. The maximum number can be computed similar to the table above:
// 7 digits: `7 * 9^6 = 3720087`
// 8 digits: `8 * 9^6 = 4251528` ==> impossible
//
// To simplify the code my loop always stops at 4251528 even though lower numbers would suffice for exponents < 6.
// Timeouts are no issue here.

#include <iostream>

int main()
{
  unsigned int exponent;
  std::cin >> exponent;

  // result
  unsigned int sum = 0;

  // there can't be a number with 8 digits (or more) which fulfils the condition for exponent=6
  // if all digits are 9s, then
  // 7 digits: 7 * 9^6 = 3720087
  // 8 digits: 8 * 9^6 = 4251528
  for (unsigned int i = 2; i <= 7*9*9*9*9*9*9; i++)
  {
    // sum of i's digits to the power of "exponent"
    unsigned int thisSum = 0;

    // split current number into its digit
    unsigned int reduce = i;
    while (reduce > 0)
    {
      // take the right-most digit
      unsigned int digit = reduce % 10;
      // and remove it
      reduce /= 10;

      // compute digit^exponent, could use pow() as well
      unsigned int power = 1;
      for (unsigned int j = 1; j <= exponent; j++)
        power *= digit;

      // add to thisSum
      thisSum += power;
    }

    // sum of digits^n equal to the original number ?
    if (thisSum == i)
      sum += i;
  }

  // and we're done
  std::cout << sum << std::endl;
  return 0;
}
