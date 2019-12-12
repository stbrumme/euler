// ////////////////////////////////////////////////////////
// # Title
// Convergents of e
//
// # URL
// https://projecteuler.net/problem=65
// http://euler.stephan-brumme.com/65/
//
// # Problem
// The square root of 2 can be written as an infinite continued fraction.
// `sqrt{2} = 1 + dfrac{1}{2 + dfrac{1}{2 + dfrac{1}{2 + frac{1}{2 + ...}}}}`
//
// The infinite continued fraction can be written, `sqrt{2} = [1;(2)]`, (2) indicates that 2 repeats ad infinitum.
// In a similar way, `sqrt{23} = [4;(1,3,1,8)]`.
//
// It turns out that the sequence of partial values of continued fractions for square roots provide the best rational approximations.
// Let us consider the convergents for `sqrt{2}`.
//
// `1 + dfrac{1}{2} = dfrac{3}{2}`
//
// `1 + dfrac{1}{2 + dfrac{1}{2}} = dfrac{7}{5}`
//
// `1 + dfrac{1}{2 + dfrac{1}{2 + dfrac{1}{2}}} = dfrac{17}{12}`
//
// `1 + dfrac{1}{2 + dfrac{1}{2 + dfrac{1}{2 + dfrac{1}{2}}}} = dfrac{41}{29}`
//
// Hence the sequence of the first ten convergents for `sqrt{2}` are:
//
// `1, dfrac{3}{2}, dfrac{7}{5}, dfrac{17}{12}, dfrac{41}{29}, dfrac{99}{70}, dfrac{239}{169}, dfrac{577}{408}, dfrac{1393}{985}, dfrac{3363}{2378}, ...`
//
// What is most surprising is that the important mathematical constant,
// `e = [2; 1,2,1, 1,4,1, 1,6,1 , ... , 1,2k,1, ...]`.
//
// The first ten terms in the sequence of convergents for `e` are:
// `2, 3, dfrac{8}{3}, dfrac{11}{4}, dfrac{19}{7}, dfrac{87}{32}, dfrac{106}{39}, dfrac{193}{71}, dfrac{1264}{465}, dfrac{1457}{536}, ...`
//
// The sum of digits in the numerator of the 10th convergent is `1+4+5+7=17`.
//
// Find the sum of digits in the numerator of the 100th convergent of the continued fraction for `e`.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Let's compare the first 10 numerators and the continuous fractions (values taken from problem statement):
//
// ||   5   ||     5     ||         10          ||
// ||   k   || numerator || continuous fraction ||
// ||   1   ||      2    ||          1          ||
// ||   2   ||      3    ||          2          ||
// ||   3   ||      8    ||          1          ||
// ||   4   ||     11    ||          1          ||
// ||   5   ||     19    ||          4          ||
// ||   6   ||     87    ||          1          ||
// ||   7   ||    106    ||          1          ||
// ||   8   ||    193    ||          6          ||
// ||   9   ||   1264    ||          1          ||
// ||  10   ||   1457    ||          1          ||
//
// After staring at the numbers for 5 minutes I saw that:
// `nominator_i = nominator_{i-2} + nominator_{i-1} * fraction_{i-1}`
//
// All I have to do is writing a simple ''for''-loop that keeps tracks of the two most recent numerators.
// The continuous fraction is `1` if `index mod 3 != 2` and `2k` if `index mod 3 == 2`.
//
// The numerators can grow pretty fast and exceed `2^64`: the numerator for `k=100` has 58 decimal digits.
// That's why I have to use my ''BigNum'' class. The code was used previously in several solutions, too, e.g. problem 56.
// There is a minor change: the constant ''MaxDigit=10'' was replaced by the highest power-of-10 that is below 2^32.
// The only reason is an improved performance (about 10x faster than ''MaxDigit = 10'').

#include <iostream>
#include <vector>

// store single digits with lowest digits first
// e.g. 1024 is stored as { 4,2,0,1 }
// only non-negative numbers supported
struct BigNum : public std::vector<unsigned int>
{
  // string conversion works only properly when MaxDigit is a power of 10
  static const unsigned int MaxDigit = 1000000000;

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

  // multiply a big number by an integer
  BigNum operator*(unsigned int factor) const
  {
    unsigned long long carry = 0;
    auto result = *this;
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
  unsigned int lastIndex;
  std::cin >> lastIndex;

  // to save memory we dont keep all numerators, only the latest three
  BigNum numerators[3] = { 0,   // dummy, will be overwritten immediately
                           1,   // always 1
                           2 }; // the first number of the continuous fraction ("before the semicolon")

  for (unsigned int index = 2; index <= lastIndex; index++)
  {
    // e = [2; 1,2,1, 1,4,1, ... 1,2k,1, ...]
    unsigned int fractionNumber = 1;
    if (index % 3 == 0)
      fractionNumber = (index / 3) * 2;

    // keep only the latest two numerators
    numerators[0] = std::move(numerators[1]);
    numerators[1] = std::move(numerators[2]);
    // and generate the next one
    if (fractionNumber == 1)
      numerators[2] = numerators[0] + numerators[1];
    else
      numerators[2] = numerators[0] + numerators[1] * fractionNumber;
  }

  // add all digits of the last numerator
  unsigned int sum = 0;
  for (auto x : numerators[2])
    // when MaxDigit != 10 then we have to split into single digits
    while (x > 0)
    {
      sum += x % 10;
      x   /= 10;
    }

  std::cout << sum << std::endl;
  return 0;
}
