// ////////////////////////////////////////////////////////
// # Title
// Power digit sum
//
// # URL
// https://projecteuler.net/problem=16
// http://euler.stephan-brumme.com/16/
//
// # Problem
// `2^{15} = 32768` and the sum of its digits is `3 + 2 + 7 + 6 + 8 = 26`.
//
// What is the sum of the digits of the number `2^{1000}`?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// These two equations describe the iterative computation of `2^x`:
// `2^0 = 1` and `2^x = 2 * 2^{x-1}`
//
// These numbers grow pretty big and for `x=64` exceed the range of ''unsigned long long''.
// That's why I store all decimal ''Digits'' in a ''std::vector'', where the lowest index contains the least significant digits.
//
// For `x=0`  - which represents `2^0 = 1`    - this ''Digits'' container's elements are ''{ 1 }''
// For `x=15` - which represents `2^{15} = 1` - this ''Digits'' container's elements are ''{ 8, 6, 7, 2, 3 }''
//
// Multiplying ''Digits'' by 2 follows the same rules as basic multiplication taught in school:
// 1. multiply each digit by 2, start at the lowest digit
// 2. if `digit * 2 >= 10` then an overflow occurred: carry over `(digit * 2) div 10` to the next digit and keep `(digit * 2) mod 10`.
//    __Note:__ we must carry over at most 1 and instead of a computationally expensive modulo we can subtract 10 which is faster
//
// # Hackerrank
// To avoid timeouts, I store all results (even intermediate steps) in a cache and re-use as much as possible from this cache.
//
// # Alternative
// [Exponentation by squaring](https://en.wikipedia.org/wiki/Exponentiation_by_squaring) saves many steps:
// The result of `2^1024` can be found in just 10 steps instead of 1024.

#include <vector>
#include <iostream>

// store single digits in an array, lowest digit come first
typedef std::vector<unsigned int> Digits;

int main()
{
  // memoize powers of two
  std::vector<Digits> cache;
  // add 2^0 = 1
  cache.push_back({ 1 });

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int exponent;
    std::cin >> exponent;

    // and compute the remaining exponents
    for (unsigned int current = cache.size(); current <= exponent; current++)
    {
      auto power = cache.back();
      unsigned int carry = 0;
      for (auto& i : power)
      {
        // times two ...
        i = 2 * i + carry;

        // handle overflow
        if (i >= 10)
        {
          i -= 10;
          carry = 1;
        }
        else
        {
          carry = 0;
        }
      }

      // still some carry left ?
      if (carry != 0)
        power.push_back(carry);

      // memoize result
      cache.push_back(power);
    }

    // sum of all digits
    unsigned int sum = 0;
    for (auto i : cache[exponent])
      sum += i;
    std::cout << sum << std::endl;
  }

  return 0;
}
