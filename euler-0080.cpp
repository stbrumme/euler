// ////////////////////////////////////////////////////////
// # Title
// Square root digital expansion
//
// # URL
// https://projecteuler.net/problem=80
// http://euler.stephan-brumme.com/80/
//
// # Problem
// It is well known that if the square root of a natural number is not an integer, then it is irrational.
// The decimal expansion of such square roots is infinite without any repeating pattern at all.
//
// The square root of two is 1.41421356237309504880..., and the digital sum of the first one hundred decimal digits is 475.
//
// For the first one hundred natural numbers, find the total of the digital sums of the first one hundred decimal digits for all the irrational square roots.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Dealing with floating-point numbers is hard, especially with the limited precision of C++.
// Therefore my program performs all computations on integers only.
// The main idea is: `sqrt{2} = sqrt{2 * frac{1000000}{1000000}} = frac{sqrt{2 * 1000000}}{1000} = 1414`
// ==> to find the first three fractional digits of `sqrt{2}` we need to "shift" 2 by `10^{2 * 3} = 10^6 = 1000000`.
//
// Computing the first 100 digits of `sqrt{2}` means that `2` must be converted to an integer with about 200 digits,
// which is obviously too much for C++'s native data types.
// It's showtime for my ''BigNum'' class - again ! About 2/3 of this solution is just ''BigNum'' code.
//
// My first experiments were based on the Newton square root algorithm (see https://en.wikipedia.org/wiki/Newton%27s_method ).
// The code worked but was too slow (mainly because of several ''BigNum'' divisions).
// Browsing through the internet people recommended a surprisingly simple and efficient digit-by-digit square root algorithm
// nicely explained in a paper by Frazer Jarvis (see http://www.afjarvis.staff.shef.ac.uk/maths/jarvisspec02.pdf ).
// That paper is quite accessible - read it if you know to know more about it.
//
// My code can be found in the function ''jarvis''. Its parameter ''precision'' is a huge number `10^{digits+extra}`.
// (due to the way this algorithm works, ''precision'' has 100 digits, not 2*100=200 digits).
// I deliberately compute more digits than required because I use a trick for faster calculations:
// only the square roots of __prime numbers__ are actually computed.
//
// For any composite number `c = a * b` where `a` and `b` are arbitrary factors of `c`, the square is
// `sqrt{c} = sqrt{a * b} = sqrt{a} * sqrt{b}`
// A single multiplication of two ''BigNum''s is much faster than a full square root computation.
//
// # Hackerrank
// I worked really hard to get 100% on this problem but I am stuck at 77.78%. Apparently only one person solved it in C++ so far.
//
// My class ''BigNum'' isn't the fastest code. I used ''operator+='' and ''operator*='' instead of ''operator+'' and ''operator*'' which helped a bit
// but I still need to be about twice as fast for the two remaining test cases.

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

  // add an integer
  void operator+=(unsigned int other)
  {
    unsigned int carry = other;
    for (size_t i = 0; i < size(); i++)
    {
      carry += operator[](i);
      if (carry == 0)
        return;

      if (carry < MaxDigit)
      {
        // no overflow
        operator[](i) = carry;
        carry         = 0;
      }
      else
      {
        // yes, we have an overflow
        operator[](i) = carry % MaxDigit;
        carry         = carry / MaxDigit;
      }
    }

    while (carry > 0)
    {
      push_back(carry % MaxDigit);
      carry /= MaxDigit;
    }
  }

  // add a big number
  void operator+=(const BigNum& other)
  {
    // add in-place, make sure it's big enough
    if (size() < other.size())
      resize(other.size(), 0);

    unsigned int carry = 0;
    for (size_t i = 0; i < size(); i++)
    {
      carry += operator[](i);
      if (i < other.size())
        carry += other[i];
      else
        if (carry == 0)
          return;

      if (carry < MaxDigit)
      {
        // no overflow
        operator[](i) = carry;
        carry     = 0;
      }
      else
      {
        // yes, we have an overflow
        operator[](i) = carry - MaxDigit;
        carry = 1;
      }
    }

    if (carry > 0)
      push_back(carry);
  }

  // subtract a smaller-or-equal number
  void operator-=(const BigNum& other)
  {
    int borrow = 0;
    for (size_t i = 0; i < size(); i++)
    {
      int diff = (int)operator[](i) - borrow;
      if (i < other.size())
        diff -= other[i];
      else
        if (borrow == 0)
          break;

      if (diff < 0)
      {
        borrow = 1;
        diff += MaxDigit;
      }
      else
        borrow = 0;

      operator[](i) = diff;
    }

    // no high zeros
    while (size() > 1 && back() == 0)
      pop_back();
  }

  // multiply a big number by an integer
  void operator*=(unsigned int factor)
  {
    // nulled
    if (factor == 0)
    {
      clear();
      push_back(0);
      return;
    }
    // unchanged
    if (factor == 1)
      return;

    // append an empty block
    if (factor == MaxDigit)
    {
      if (size() > 1 || operator[](0) > 0)
        insert(begin(), 0);
      return;
    }

    // multiply all blocks with the factor
    unsigned long long carry = 0;
    for (auto& i : *this)
    {
      carry += i * (unsigned long long)factor;
      i      = carry % MaxDigit;
      carry /= MaxDigit;
    }
    // store remaining carry in new digits
    while (carry > 0)
    {
      push_back(carry % MaxDigit);
      carry /= MaxDigit;
    }
  }

  // multiply two big numbers
  BigNum operator*(const BigNum& other) const
  {
    if (size() < other.size())
      return other * *this;

    // multiply single digits of "other" with the current object
    BigNum result = 0;
    result.reserve(size() + other.size());
    for (int i = (int)other.size() - 1; i >= 0; i--)
    {
      BigNum temp = *this;
      temp   *= other[i];

      result *= MaxDigit;
      result += temp;
    }

    return result;
  }

  // compare two big numbers
  bool operator<(const BigNum& other) const
  {
    // different number of digits/buckets ?
    if (size() < other.size())
      return true;
    if (size() > other.size())
      return false;
    // compare all digits/buckets, beginning with the most significant
    for (int i = (int)size() - 1; i >= 0; i--)
    {
      if (operator[](i) < other[i])
        return true;
      if (operator[](i) > other[i])
        return false;
    }
    return false;
  }

  // convert to string, MaxDigit must be power of 10
  std::string toString() const
  {
    std::string result;
    for (auto x : *this)
    {
      // process a bucket
      for (unsigned int shift = 1; shift < MaxDigit; shift *= 10)
      {
        auto digit = (x / shift) % 10;
        result.insert(0, 1, (char)digit + '0');
      }
    }

    // remove leading zeros
    while (result.size() > 1 && result.front() == '0')
      result.erase(0, 1);

    return result;
  }
};


// find square root of x
// see http://www.afjarvis.staff.shef.ac.uk/maths/jarvisspec02.pdf
BigNum jarvis(unsigned int x, const BigNum& precision)
{
  static const BigNum Fortyfive = 45;

  BigNum a = x * 5;
  BigNum b = 5;

  // avoid re-allocations when growing (plus a few bytes when exceeding target)
  a.reserve(precision.size());
  b.reserve(precision.size());

  // until we have enough digits
  while (b < precision)
  {
    if (!(a < b)) // same as a >= b but currently there is no operator >= in my BigNum class
    {
      a -=   b;
      b +=  10;
    }
    else
    {
      a *= 100;
      b *=  10;
      b -= Fortyfive;
    }
  }

  return b;
}

// for reference only:
// Newton's method
//BigNum newton(const BigNum& square, BigNum estimate)
//{
//  while (true)
//  {
//    // minimize error
//    auto next = (square + estimate * estimate) / (estimate * 2);
//    if (next == estimate)
//      return estimate;
//    estimate = next;
//  }
//}

// return the sum of the first digits
unsigned int countDigits(const BigNum& x, unsigned int numDigits)
{
  unsigned int sum = 0;
  for (auto i : x.toString().substr(0, numDigits))
    sum += i - '0';
  return sum;
}

int main()
{
  // all square roots from 2 to maxNumber
  unsigned int maxNumber = 100;
  // the first digits include the integer part, too (first six digits of sqrt(2) are 1,4,1,4,2,1)
  unsigned int digits    = 100;
  std::cin >> maxNumber >> digits;

  // precompute precision
  // a single 1 and then 100 zeros
  // => since BigNum stores everything in reverse it is technically 100 zeros and then a single 1
  // actually we need a slightly higher precision because square roots of composite numbers will be computed as
  // the product of the square roots of their prime factors
  const unsigned int ExtraDigits = 15; // maybe I can get away with less extra digits, haven't tried it
  BigNum precision = 10;
  for (unsigned int i = 1; i < digits + ExtraDigits; i++)
    precision *= 10;

  // all square roots (initially 0)
  std::vector<BigNum> roots(maxNumber + 1, 0);

  // digits(sqrt(2)) + digits(sqrt(3)) + ...
  unsigned int sum = 0;
  for (unsigned int i = 1; i <= maxNumber; i++)
  {
    // handle perfect squares
    unsigned int intSqrt = 1;
    while (intSqrt * intSqrt < i)
      intSqrt++;
    // found a perfect square ?
    if (intSqrt * intSqrt == i)
    {
      // compute its root, too (needed for roots of composite numbers)
      roots[i] = precision * intSqrt;
      continue;
    }

    // try to number split into a product (only possible for composite numbers)
    auto factor = intSqrt - 1;
    // find a factor close to the square root
    while (i % factor != 0)
      factor--;

    // composite ?
    if (factor > 1)
    {
      // sqrt(composite) = sqrt(factor) * sqrt(i / factor)
      auto& current = roots[i] = roots[i / factor] * roots[factor];

      // remove digits beyond required precision
      if (current.size() > roots[i - 1].size())
        current.erase(current.begin(), current.begin() + (current.size() - roots[i - 1].size()));
      while (current < roots[i - 1])
        current *= 10;
    }
    else
    {
      // number is prime ? must find the square root the hard way ...
      roots[i] = jarvis(i, precision);
    }

    // compute digit sum of sqrt(i)
    sum += countDigits(roots[i], digits);
  }

  // print result
  std::cout << sum << std::endl;

  return 0;
}
