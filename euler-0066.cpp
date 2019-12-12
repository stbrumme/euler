// ////////////////////////////////////////////////////////
// # Title
// Diophantine equation
//
// # URL
// https://projecteuler.net/problem=66
// http://euler.stephan-brumme.com/66/
//
// # Problem
// Consider quadratic Diophantine equations of the form:
// `x^2 - D * y^2 = 1`
//
// For example, when `D=13`, the minimal solution in `x` is `649^2 - 13 * 1802 = 1`.
//
// It can be assumed that there are no solutions in positive integers when `D` is square.
//
// By finding minimal solutions in `x` for `D = { 2, 3, 5, 6, 7 }`, we obtain the following:
//
// `3^2 - 2 * 2^2 = 1`
// `2^2 - 3 * 1^2 = 1`
// `9^2 - 5 * 4^2 = 1`
// `5^2 - 6 * 2^2 = 1`
// `8^2 - 7 * 3^2 = 1`
//
// Hence, by considering minimal solutions in `x` for `D <= 7`, the largest `x` is obtained when `D=5`.
//
// Find the value of `D <= 1000` in minimal solutions of `x` for which the largest value of `x` is obtained.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// I didn't know anything about Pell's equation before I started solving this problem.
// There is a Wikipedia article about it ( https://en.wikipedia.org/wiki/Pell%27s_equation) where it becomes obvious that some numbers will be large.
// In fact, too large for C++'s native data types. That means that my ''BigNum'' class (see my [toolbox](../toolbox/) ) has to be used.
//
// My program computes the continuous fractions of `x` and `y` (see problem 64) and stops as soon as it finds a solution:
// `x^2 - D * y^2 = 1`
// I wasn't willing to add the code for subtraction to my ''BigNum'' class because the formula can be rewritten as:
// `x^2 = 1 + D * y^2`

#include <cmath>
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
    // faster multiplication possible ?
    if (factor == 0)
      return 0;
    if (factor == 1)
      return *this;
    if (factor == MaxDigit)
    {
      auto result = *this;
      result.insert(result.begin(), 0);
      return result;
    }
    // might be slower but avoids nasty overflows
    if (factor > MaxDigit)
      return *this * BigNum(factor);

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

  // multiply two big numbers
  BigNum operator*(const BigNum& other) const
  {
    // multiply single digits of "other" with the current object
    BigNum result = 0;
    for (int i = (int)other.size() - 1; i >= 0; i--)
      result = result * MaxDigit + (*this * other[i]);

    return result;
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
};

int main()
{
  unsigned int limit = 1000;
  std::cin >> limit;

  // initial solutions
  unsigned int bestD = 2;
  BigNum bestX = 3;

  // solve for all values of D
  for (unsigned int d = 3; d <= limit; d++)
  {
    unsigned int root = sqrt(d);
    // exclude squares
    if (root * root == d)
      continue;

    // see problem 64
    unsigned int a = root;
    unsigned int numerator   = 0;
    unsigned int denominator = 1;

    // keep only the most recent 3 numerators and denominators while diverging
    BigNum x[3] = { 0, 1, root }; // numerators
    BigNum y[3] = { 0, 0, 1 };    // denominators

    // find better approximations until the exact solution is found
    while (true)
    {
      numerator   = denominator * a - numerator;
      denominator = (d - numerator * numerator) / denominator;
      a = (root + numerator) / denominator;

      // x_n = a * x_n_minus_1 + x_n_minus_2
      x[0] = std::move(x[1]);
      x[1] = std::move(x[2]);
      x[2] = x[1] * a + x[0];

      // y_n = a * y_n_minus_1 + y_n_minus_2
      y[0] = std::move(y[1]);
      y[1] = std::move(y[2]);
      y[2] = y[1] * a + y[0];

      // avoid subtraction (to keep BigNum's code short)
      // x*x - d*y*y = 1
      // x*y         = 1 + d*y*y
      auto leftSide  = x[2] * x[2];
      auto rightSide = y[2] * y[2] * d + 1;

      // solved it
      if (leftSide == rightSide)
        break;
    }

    // biggest x so far ?
    if (bestX < x[2])
    {
      bestX = x[2];
      bestD = d;
    }
  }

  // print D where x was maximized
  std::cout << bestD << std::endl;
  return 0;
}
