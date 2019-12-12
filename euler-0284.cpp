// ////////////////////////////////////////////////////////
// # Title
// Steady Squares
//
// # URL
// https://projecteuler.net/problem=284
// http://euler.stephan-brumme.com/284/
//
// # Problem
// The 3-digit number 376 in the decimal numbering system is an example of numbers with the special property that its square ends with the same digits: `376^2 = 141376`.
// Let's call a number with this property a steady square.
//
// Steady squares can also be observed in other numbering systems. In the base 14 numbering system, the 3-digit number c37 is also a steady square: `c37^2 = aa0c37`,
// and the sum of its digits is c+3+7=18 in the same numbering system.
// The letters a, b, c and d are used for the 10, 11, 12 and 13 digits respectively, in a manner similar to the hexadecimal numbering system.
//
// For `1 <= n <= 9`, the sum of the digits of all the `n`-digit steady squares in the base 14 numbering system is 2d8 (582 decimal).
// Steady squares with leading 0's are not allowed.
//
// Find the sum of the digits of all the n-digit steady squares in the base 14 numbering system for
// `1 <= n <= 10000` (decimal) and give your answer in the base 14 system using lower case letters where necessary.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// This problem bugged me for a while ... the amount of code written for my solution was astonishing (and only a fraction of it survived ...).
//
// Nevertheless, here's how I did it:
// Within a few seconds I found exactly three single steady digits:
// `1^2 mod 14 = 1`
// `7^2 mod 14 = 49 mod 14 == 7`
// `8^2 mod 14 = 64 mod 14 == 8`
// (zero was disallowed by the problem statement)
//
// Continuing with two digits caused a problem:
// I couldn't find a digit that can be prepended to 1 - but there are such digits for 7 and 8.
// As it turns out, I can "extend" 7 and 8 basically to infinitely long steady squares.
// And there is always exactly one digit I can prepend to a steady square such that it becomes a larger steady square.
// The ''BigNum'' class from my [toolbox](../toolbox/) already supported different bases.
// Adding a ''toString()'' method for base 14 was straightforward.
// Then I wrote ''isSteady()'': it multiplies the number with itself and in each step checks whether the last digits remain unchanged.
//
// The obvious brute force algorithm is as follows:
// - prepend every possible digit `d = 0,1,2,3,...9,a,...` to a steady square `s` of length `m` so that `s' = d * 14^m + s`
// - compute its square `t = s' * s'`
// - if the last `m+1` digits of `t` are identical to `s'` then I found the correct digit
// It works pretty well for up to 100 digits.
//
// The Wikipedia page https://en.wikipedia.org/wiki/Automorphic_number focusses on an algorithm based on the Chinese Remainder Theorem.
// And I don't really like it ... so it looked at the output and saw that there is a certain relationship between the first steady squares of 7:
// `s_1 =     7_14 =      7_10`
// `s_2 =    37_14 =     49_10 =  7^2`
// `s_3 =   c37_14 =   2401_10 = 49^2 = 7^4`
// `s_4 =  0c37_14 =   2401_10 = 49^2 = 7^4`
// `s_5 = a0c37_14 = 386561_10 = 7^5 * 23`
// Unfortunately it doesn't work for `s_4` and `s_5` anymore, so I stopped working and solved other Project Euler problems.
// Problem 455 isn't related to the current problem but it reminded my that modulo often works in strange ways.
// And indeed:
// `s_4 = s^2_3 mod 14^4 = 2401^2 mod 14^4 = 5764801 mod 38416 == 2401`
// `s_5 = s^2_4 mod 14^5 = 2401^2 mod 14^5 = 2401 mod 537824 == 386561`
// So I have a simple way to generate all digits step-by-step.
//
// My multiplication code was extremely slow: the square of a number with `m` digits has `2m` digits.
// But I only need `m + 1` of those - so I wrote ''multiplyLow()'' which stops after enough digits were computed and discards the rest.
//
// The Wikipedia mentions a smart way to generate the second automorphic number: "the sum of two automorphic numbers is `10^k + 1`".
// If I have one automorphic number `s` then the other one is `S = 10^k + 1 - s`. My ''findOther()'' function does exactly that.
// The solution for 1000 digits takes half a second and the solution for 10000 digits is found after about 8 minutes.
//
// But another formula on the Wikipedia page caught my attention (I didn't realize its importance when I read it the first time):
// `n' = (3n^2 - 2n^3) mod 10^{2k}`
// Starting with an automorphic number `n` I get a new automorphic number `n'` that has __twice__ as many digits.
// Even though this formula is intended for base 10 numbers it still works for base 14 (I have no idea about other bases).
//
// My ''BigNum'' class supports only positive numbers and `3n^2 - 2n^3` is always negative for `n > 1`.
// So I had to rewrite the formula using the relationship `-a mod b = (b-a) mod b`:
// `n' =  (3n^2 - 2n^3) mod 10^{2k}`
// `n' = -(2n^3 - 3n^2) mod 10^{2k}`
// `n' = (10^{2k} - (2n^3 - 3n^2)) mod 10^{2k}`
//
// See my ''fastDoubling()'' function ==> it finishes after 1.1 seconds.
//
// The final step is to compute the digit sum of my steady squares:
// - the last digit is part of all steady squares, that means it appears 10000 times
// - the second-to-last digit is part of all steady squares, except the steady square with only one digit, that means it appears 9999 times
// - ... and so on ...
// - but any steady square with a leading zero must be discarded ==> I subtract all digits in a simple loop (looks slow, but it surprisingly fast)
//
// # Note
// This solution currently shares the "first place" of most code I needed to solve a problem, see my [ranking](../performance/#codemetrics).
// Despite all the code (and time) needed to crack the problem, I felt that this was a really nice one.
//
// # Alternative
// Dedicated large integer library such as GMP are much faster than my simple ''BigNum'' class.
// With a clever use of residues you can get away with simple integer arithmetic as well and solve the problem in less than 0.1 seconds.

#include <iostream>
#include <vector>
#include <string>

// ---------- code taken from my toolbox, with a few modifications / extensions, see explanations above ----------

// store single digits with lowest digits first
// e.g. 1024 (in base 14) is stored as { 4,2,0,1 }
// only non-negative numbers supported
struct BigNum : public std::vector<unsigned char>
{
  static const unsigned int MaxDigit = 14;

  // store a non-negative number
  BigNum(unsigned int x = 0)
  {
    do
    {
      push_back(x % MaxDigit);
      x /= MaxDigit;
    } while (x > 0);
  }

  // convert from base 14 to string
  std::string toString() const
  {
    const char digits[] = "0123456789abcd";

    std::string result;
    for (auto x : *this)
    {
      // process a bucket
      for (unsigned int shift = 1; shift < MaxDigit; shift *= MaxDigit)
      {
        auto digit = (x / shift) % MaxDigit;
        result.insert(0, 1, digits[digit]);
      }
    }

    // remove leading zeros
    while (result.size() > 1 && result.front() == '0')
      result.erase(0, 1);

    return result;
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
    // faster multiplication possible ?
    if (factor == 0)
      return 0;
    if (factor == 1)
      return *this;
    if (factor == MaxDigit)
    {
      if (size() == 1 && operator[](0) == 0)
        return 0;

      auto result = *this;
      result.insert(result.begin(), 0);
      return result;
    }

    unsigned int carry = 0;
    auto result = *this;
    for (auto& i : result)
    {
      carry += i * factor;
      i      = carry % MaxDigit;
      carry /= MaxDigit;
    }
    // store remaining carry in new digits
    while (carry > 0)
    {
      result.push_back(carry % MaxDigit);
      carry /= MaxDigit;
    }

    return std::move(result);
  }

  // subtract a smaller-or-equal number
  BigNum operator-(const BigNum& other) const
  {
    BigNum result = *this;
    int borrow = 0;
    for (size_t i = 0; i < result.size(); i++)
    {
      int diff = (int)result[i] - borrow;
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

      result[i] = diff;
    }

    // no high zeros
    while (result.size() > 1 && result.back() == 0)
      result.pop_back();

    return result;
  }

  // multiply two big numbers
  BigNum operator*(const BigNum& other) const
  {
    if (size() < other.size())
      return other * *this;

    // multiply single digits of "other" with the current object
    BigNum result = 0;
    for (int i = (int)other.size() - 1; i >= 0; i--)
      result = result * MaxDigit + (*this * other[i]);

    return std::move(result);
  }

// ---------- problem-specific code (still part of BigNum class) ----------

  // multiply by a big number and keep only the lowest digits
  void multiplyLow(const BigNum& other, size_t numDigits)
  {
    BigNum result;
    result.resize(numDigits, 0);

    // multiply single digits of "other" with the current object
    unsigned int carry = 0;
    for (size_t i = 0; i < other.size() && i < numDigits; i++)
    {
      carry = 0;
      for (size_t j = 0; i + j < numDigits; j++)
      {
        // multiply two digits (if possible) and add carry
        if (j >= size())
          carry += result[i + j];
        else
          carry += result[i + j] + other[i] * operator[](j);

        // store lowest digit of product and keep "carrying on" :-)
        result[i + j] = carry % MaxDigit;
        carry /= MaxDigit;
      }
    }

    *this = std::move(result);
  }

  // try to find the square and check whether its lower digits remain unchanged
  bool isSteady() const
  {
    // multiply the number with itself and abort as soon as possible
    BigNum square = 0;

    for (size_t pos = 0; pos < size(); pos++)
    {
      // multiply with next digit
      auto digit = operator[](pos);
      if (digit > 0)
      {
        auto product = *this * digit;

        for (size_t appendZeros = 0; appendZeros < pos; appendZeros++)
          product.insert(product.begin(), 0);
        square = square + product;
      }

      // mismatch ?
      if (digit != square[pos])
        return false;
    }

    // yes, a steady square
    return true;
  }
};

// ---------- problem-specific code (not part of BigNum class) ----------

// prepend every possible digit until a steady square is fouund
BigNum bruteForce(const BigNum& number)
{
  auto next = number;
  next.push_back(0);

  // prepend a digit (remember: digits are stored in reverse order)
  for (unsigned int digit = 0; digit < BigNum::MaxDigit; digit++)
  {
    next.back() = digit;
    // steady ?
    if (next.isSteady())
      break;
  }
  return next;
}

// compute steady square where the right-most digits are "number", result has exactly "numDigits" digits
BigNum fastDoubling(const BigNum& number, unsigned int numDigits)
{
  // https://en.wikipedia.org/wiki/Automorphic_number
  // n' = (3n^2 - 2n^3) mod 14^2k          => a negative number, which my simple BigNum class doesn't support yet
  // n' = 14^2k - (2n^3 - 3n^2) mod 14^2k

  // after each iteration, the number of steady digits will double
  auto current = number;
  while (current.size() < numDigits)
  {
    // new number will have twice as many digits
    auto twiceDigits = 2 * current.size();

    auto square = current * current;     // n^2
    auto cube   = square  * current;     // n^3
    auto diff   = cube * 2 - square * 3; // 3n^2 - 2n^3

    diff.resize(twiceDigits);            // (3n^2 - 2n^3) mod 14^twiceDigits

    BigNum largeOne = 0;
    largeOne.resize(twiceDigits, 0);     // 14*twiceDigits zeros
    largeOne.push_back(1);               // and prepend a "1" => 14^twiceDigits

    current = largeOne - diff;           // 14^twiceDigits - (3n^2 - 2n^3) mod 14^twiceDigits
  }

  // remove superfluous leading digits (current has always 2^iterations digits, e.g. 16384 which will be reduced to 10000)
  current.resize(numDigits);
  return current;
}

// given an automorphic number, find its sibling
BigNum findOther(const BigNum& number)
{
  // eight = (14^maxDigits + 1) - seven
  BigNum one0one = 1;
  one0one.resize(number.size(), 0);
  one0one.push_back(1);
  return one0one - number;
}

int main()
{
  unsigned int maxDigits = 10000;
  std::cin >> maxDigits;

  // manually found those two number to be the only steady digits mod 14 (except for 1, see below)
  BigNum seven = 7;
  BigNum eight = 8;

//#define BRUTEFORCE
#ifdef  BRUTEFORCE
  // brute-force
  for (unsigned int i = 2; i < maxDigits; i++)
  {
    seven = bruteForce(seven);
    eight = bruteForce(eight);
  }
#endif

//#define POWER
#ifdef  POWER
  for (unsigned int numDigits = 2; numDigits <= maxDigits; numDigits++)
  {
    // seven = seven * seven, reduced to numDigits
    seven.multiplyLow(seven, numDigits);
    //std::cout << numDigits << " " << std::flush;
  }
  eight = findOther(seven);
#endif

#define DOUBLING
#ifdef  DOUBLING
  // use equation (3n^2 - 2n^3) mod 14^k until I have 10000 digits
  seven = fastDoubling(7, maxDigits);
  eight = findOther(seven); // faster than eight = fastDoubling(8, maxDigits);
#endif

  // now I have the 10000 steady digits ending with 7 and 8, let's find their digit sum !

  // 1 is a steady digit, too, but a strange special case: 1^2 % 14 = 1
  unsigned int sum = 1;
  // prepending any digit to 1 doesn't produce a steady square

  // okay, let's proceed with ...7 and ...8
  for (unsigned int i = 0; i < maxDigits; i++)
  {
    // last digit appears in 10000 steady squares, next to last in 9999 steady squares, ...
    auto howOften = maxDigits - i;
    sum += howOften * seven[i];
    sum += howOften * eight[i];

    // but don't count those numbers having a leading zero
    if (seven[i] == 0)
      for (unsigned int j = 0; j < i; j++) // could be more efficient ... but it's still too fast to be measurable
        sum -= seven[j];

    if (eight[i] == 0)
      for (unsigned int j = 0; j < i; j++)
        sum -= eight[j];
  }

  // print digit sum in base 14
  BigNum sumBase14 = sum;
  std::cout << sumBase14.toString() << std::endl;
  return 0;
}
