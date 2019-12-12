// ////////////////////////////////////////////////////////
// # Title
// Powerful digit sum
//
// # URL
// https://projecteuler.net/problem=56
// http://euler.stephan-brumme.com/56/
//
// # Problem
// A googol (`10^100`) is a massive number: one followed by one-hundred zeros; `100^100` is almost unimaginably large: one followed by two-hundred zeros.
// Despite their size, the sum of the digits in each number is only 1.
//
// Considering natural numbers of the form, ab, where a, b < 100, what is the maximum digital sum?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// I wrote a small class ''BigNum'' that handles arbitrarily large integers (only positive, no sign).
// It supports multiplication based on the simple algorithm that you'd use with pen and paper, too.
//
// My inner loop has a variable called ''power'' which represents `base^{exponent}`. Then `base^{exponent+1} = base^{exponent} * base`.
// The digit sum iterates over all digits and keeps track of the largest sum (''maxSum'').

#include <vector>
#include <iostream>

// store single digits with lowest digits first
// e.g. 1024 is stored as { 4,2,0,1 }
// only non-negative numbers supported
struct BigNum : public std::vector<unsigned int>
{
  // must be 10 for this problem: a single "cell" store one digit 0 <= digit < 10
  static const unsigned int MaxDigit = 10;

  // store a non-negative number
  BigNum(unsigned long long x = 0)
  {
    // actually the constructor is always called with x = 1, but I keep my default implementation
    do
    {
      push_back(x % MaxDigit);
      x /= MaxDigit;
    } while (x > 0);
  }

  // multiply a big number by an integer
  BigNum operator*(unsigned int factor) const
  {
    unsigned long long carry = 0;
    auto result = *this;
    // multiply each block by the number, take care of temporary overflows (carry)
    for (auto& i : result)
    {
      carry += i * (unsigned long long)factor;
      i      = carry % MaxDigit;
      carry /= MaxDigit;
    }
    // store remaining carry in new digits
    while (carry > 0)
    {
      result.push_back(carry % MaxDigit);
      carry /= MaxDigit;
    }

    return result;
  }
};

int main()
{
  // maximum base/exponent (100 for Googol)
  unsigned int maximum = 100;
  std::cin >> maximum;

  // look at all i^j
  unsigned int maxSum = 1;
  for (unsigned int base = 1; base <= maximum; base++)
  {
    // incrementally compute base^exponent
    BigNum power = 1;
    for (unsigned int exponent = 1; exponent <= maximum; exponent++)
    {
      // add all digits
      unsigned int sum = 0;
      for (auto digit : power)
        sum += digit;

      // new world record ? ;-)
      if (maxSum < sum)
        maxSum = sum;

      // same base, next exponent:
      // base^(exponent + 1) = (base^exponent) * base
      power = power * base;
    }
  }

  std::cout << maxSum << std::endl;
  return 0;
}
