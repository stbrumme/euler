// ////////////////////////////////////////////////////////
// # Title
// Square root convergents
//
// # URL
// https://projecteuler.net/problem=57
// http://euler.stephan-brumme.com/57/
//
// # Problem
// It is possible to show that the square root of two can be expressed as an infinite continued fraction.
// `sqrt{2} = 1 + frac{1}{2 + frac{1}{2 + frac{1}{2 + ... }}} = 1.414213...`
//
// By expanding this for the first four iterations, we get:
//
// `1 + dfrac{1}{2} = dfrac{3}{2} = 1.5`
//
// `1 + dfrac{1}{2 + frac{1}{2}} = dfrac{7}{5} = 1.4`
//
// `1 + dfrac{1}{2 + frac{1}{2 + frac{1}{2}}} = dfrac{17}{12} = 1.41666...`
//
// `1 + dfrac{1}{2 + frac{1}{2 + frac{1}{2 + frac{1}{2}}}} = dfrac{41}{29} = 1.41379...`
//
// The next three expansions are `dfrac{99}{70}` , `dfrac{239}{169}`, and `dfrac{577}{408}`, but the eighth expansion, `dfrac{1393}{985}`,
// is the first example where the number of digits in the numerator exceeds the number of digits in the denominator.
//
// In the first one-thousand expansions, how many fractions contain a numerator with more digits than denominator?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// An iteration/expansion can be described as:
// `f(n+1) = 1 + dfrac{1}{2 + f(n)}`
//
// Each iteration/expansion is a fraction, let's call it `f(n) = dfrac{a(n)}{b(n)}`
// `f(n+1) = 1 + dfrac{1}{1 + frac{a(n)}{b(n)}}`
// `       = 1 + dfrac{1}{frac{b(n) + a(n)}{b(n)}}`
//
// `       = 1 + dfrac{b(n)}{b(n) + a(n)}`
//
// `       = dfrac{b(n) + a(n) + b(n)}{b(n) + a(n)}`
//
// `dfrac{a(n+1)}{b(n+1)} = dfrac{2b(n) + a(n)}{b(n) + a(n)}`
//
// so it's actually pretty easy to continuously compute numerator and denominator:
// `a(n+1) = 2b(n) + a(n)`
// `b(n+1) =  b(n) + a(n)`
//
// inital values:
// `x(0)   = 1 + 1/2 = 1 + 1/(1+1)`
// `a(0)   = 1`
// `b(0)   = 1`
//
// The ''BigNum'' was copied from problem 56. When ''MaxDigit = 10'' then each element of the array is one digit and I can compare ''a.size() > b.size()''.
//
// # Hackerrank
// I have to print each iteration's ID where the numerator has more digits than the denominator.

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
    do
    {
      push_back(x % MaxDigit);
      x /= MaxDigit;
    } while (x > 0);
  }

  // add two big numbers
  BigNum operator+(const BigNum& other) const
  {
    auto result = *this;
    // add in-place, make sure it's big enough
    if (result.size() < other.size())
      result.resize(other.size(), 0);

    unsigned int carry = 0;
    for (size_t i = 0; i < result.size(); i++)
    {
      carry += result[i];
      if (i < other.size())
        carry += other[i];
      else
        if (carry == 0)
          return result;

      if (carry < MaxDigit)
      {
        // no overflow
        result[i] = carry;
        carry     = 0;
      }
      else
      {
        // yes, we have an overflow
        result[i] = carry - MaxDigit;
        carry = 1;
      }
    }

    if (carry > 0)
      result.push_back(carry);

    return result;
  }
};

#define ORIGINAL

int main()
{
  unsigned int iterations = 1000;
  std::cin >> iterations;

  // both values have one digit initialized with 1
  BigNum a = 1;
  BigNum b = 1;

  unsigned int count = 0;
  for (unsigned int i = 0; i <= iterations; i++)
  {
    // different number of digits ?
    if (a.size() > b.size())
    {
#ifdef ORIGINAL
      count++;
#else
      std::cout << i << std::endl;
#endif
    }

    // a(n+1) = 2*b(n) + a(n)
    // b(n+1) =   b(n) + a(n)
    auto twoB  = b + b;
    auto nextA = a + twoB;
    auto nextB = b + a;

    a = std::move(nextA);
    b = std::move(nextB);
  }

#ifdef ORIGINAL
  std::cout << count << std::endl;
#endif

  return 0;
}
