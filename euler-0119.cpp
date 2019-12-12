// ////////////////////////////////////////////////////////
// # Title
// Digit power sum
//
// # URL
// https://projecteuler.net/problem=119
// http://euler.stephan-brumme.com/119/
//
// # Problem
// The number 512 is interesting because it is equal to the sum of its digits raised to some power: `5 + 1 + 2 = 8`, and `8^3 = 512`.
// Another example of a number with this property is `614656 = 28^4`.
//
// We shall define `a_n` to be the nth term of this sequence and insist that a number must contain at least two digits to have a sum.
//
// You are given that `a_2 = 512` and `a_10 = 614656`.
//
// Find `a_30`.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// A simple but extremely slow brute-force program revealed that `a_30` has more than 10 digits:
// I was iterating through numbers from 10 to 10,000,000,000 and didn't find enough terms `a_i`:
// 81, 512, 2401, 4913, 5832, 17576, 19683, 234256, 390625, 614656, 1679616, 17210368, 34012224, 52521875, 60466176, 205962976, 612220032, 8303765625, ...
//
// Then I turned the whole problem upside-down: there are 90 two-digit numbers but only 18 different digit sums.
// Even better: there are 9000 four-digit numbers but only 36 different digit sums !
// Actually each digit is always between 0 and 9 therefore the maximum digit sum is `sum_i{digit_i} = 9i`.
//
// My program looks at all potential digit sums from 1 to 9*20 (I thought that maybe `a_30 < 10^20`, and that's actually true).
// For each potential digit sum, I call them ''base'' in my code, all exponents are computed until `base^{exponent}` overflows.
// If `digitSum(base^{exponent}) = base` then a solution has been found.
//
// Solutions will be found in a "random" order, so I add them to a sorted ''std::set'' and read its 30th element.
//
// # Hackerrank
// The search space is much bigger: all numbers up to `10^100` (that's a [Googol](https://en.wikipedia.org/wiki/Googol)) have to be printed.
// Even worse, numbers can be encoded in any base from 2 to 1000.
//
// My ''BigNum'' class was already able to store its digits in arbirary bases - but I always had powers of 10 in my mind.
// Addition and multiplication is reduced to in-place operations to avoid slow memory allocations.
// Pretty simple but effective is ''BigNum::convert'' which converts a ''BigNum'' to a different base.

#include <iostream>
#include <set>
#include <climits>

//#define ORIGINAL
#ifdef ORIGINAL

// add all digits (base 10)
unsigned int digitSum(unsigned long long x)
{
  unsigned int result = 0;
  while (x > 0)
  {
    result += x % 10;
    x      /= 10;
  }
  return result;
}

int main()
{
  std::set<unsigned long long> solutions;

  // based on my experiments with BigNum I knew that the result fits in 64 bit
  // which is about 10^20, each digit <= 9, therefore their sum <= 20*9
  unsigned int MaxBase = 20 * 9;
  // base is the sum of all digits
  for (unsigned int base = 2; base <= MaxBase; base++)
  {
    unsigned int exponent = 1;
    // step-by-step: base^exponent, then base^(exponent+1), ...
    unsigned long long current = base;

    while (current < ULLONG_MAX / base) // 1^64 - 1
    {
      auto sum = digitSum(current);
      // digit sum equals base ? (note: single-digit numbers excluded)
      if (sum == base && current >= 10)
        solutions.insert(current);

      // next iteration
      current *= base;
      exponent++;
    }
  }

  // sorted results
  auto i = solutions.begin();
  // skip 30 - 1 = 29 elements
  std::advance(i, 30 - 1);
  // print the 30th
  std::cout << *i << std::endl;
  return 0;
}

#else

#include <vector>

// store single digits with lowest digits first
// e.g. 1024 is stored as { 4,2,0,1 } (when base = 10)
struct BigNum : public std::vector<unsigned int>
{
  unsigned int maxDigit;

  // store a non-negative number
  BigNum(unsigned long long x = 0, unsigned int base = 10)
  : maxDigit(base)
  {
    do
    {
      push_back(x % maxDigit);
      x /= maxDigit;
    } while (x > 0);
  }

  // add two big numbers
  void operator+=(const BigNum& other)
  {
    // add in-place, make sure it's big enough
    if (size() < other.size())
      resize(other.size(), 0);

    unsigned int carry = 0;
    for (unsigned int i = 0; i < size(); i++)
    {
      carry += operator[](i);
      if (i < other.size())
        carry += other[i];
      else
        if (carry == 0)
          return;

      if (carry < maxDigit)
      {
        // no overflow
        operator[](i) = carry;
        carry = 0;
      }
      else
      {
        // yes, we have an overflow
        operator[](i) = carry - maxDigit;
        carry = 1;
      }
    }

    if (carry > 0)
      push_back(carry);
  }

  // multiply a big number by an integer
  void operator*=(unsigned int factor)
  {
    unsigned long long carry = 0;
    for (size_t i = 0; i < size(); i++)
    {
      carry += operator[](i) * (unsigned long long)factor;
      operator[](i) = carry % maxDigit;
      carry /= maxDigit;
    }
    // store remaining carry in new digits
    while (carry > 0)
    {
      push_back(carry % maxDigit);
      carry /= maxDigit;
    }
  }

  // compare two big numbers
  bool operator<(const BigNum& other) const
  {
    if (size() < other.size())
      return true;
    if (size() > other.size())
      return false;
    for (int i = (int)size() - 1; i >= 0; i--)
    {
      if (operator[](i) < other[i])
        return true;
      if (operator[](i) > other[i])
        return false;
    }
    return false;
  }

  // convert to a different radix
  BigNum convert(unsigned int newRadix) const
  {
    BigNum result(0, newRadix);
    for (auto i = rbegin(); i != rend(); i++)
    {
      result *= maxDigit;
      result += *i;
    }
    return result;
  }
};


// add all digits
unsigned int digitSum(const BigNum& x)
{
  unsigned int result = 0;
  for (auto digit : x)
    result += digit;
  return result;
}


int main()
{
  std::set<BigNum> solutions;
  unsigned int radix = 10;
  std::cin >> radix;

  // do not exceed 10^100
  BigNum googol(1, 10);
  for (unsigned int digits = 1; digits <= 100; digits++)
    googol *= 10;
  // convert to current radix
  BigNum max = googol.convert(radix);

  // analyze all base^exponent below 10^100
  // base is the sum of digits
  for (unsigned int base = 2; base < (radix-1)*max.size(); base++)
  {
    unsigned int exponent = 1;
    // step-by-step: base^exponent, then base^(exponent+1), ...
    BigNum current(base, radix);
    // still below a googol
    while (current < max)
    {
      auto sum = digitSum(current);
      // digit sum equals base ? single-digit numbers excluded
      if (sum == base && current.size() >= 2)
      {
        // store result in decimal notation
        BigNum decimal = current.convert(10);
        solutions.insert(decimal);
      }

      // next iteration
      current *= base;
      exponent++;
    }
  }

  // sorted results
  for (auto i : solutions)
  {
    // print digits (note: they are stored in reverse)
    for (auto j = i.rbegin(); j != i.rend(); j++)
      std::cout << *j;
    std::cout << " ";
  }

  return 0;
}
#endif
