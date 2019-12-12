// ////////////////////////////////////////////////////////
// # Title
// Ordered fractions
//
// # URL
// https://projecteuler.net/problem=71
// http://euler.stephan-brumme.com/71/
//
// # Problem
// Consider the fraction, `dfrac{n}{d}`, where `n` and `d` are positive integers. If `n<d` and `HCF(n,d)=1`, it is called a reduced proper fraction.
//
// If we list the set of reduced proper fractions for `d <= 8` in ascending order of size, we get:
//
// `dfrac{1}{8}`, `dfrac{1}{7}`, `dfrac{1}{6}`, `dfrac{1}{5}`, `dfrac{1}{4}`, `dfrac{2}{7}`, `dfrac{1}{3}`, `dfrac{3}{8}`, `dfrac{2}{5}`, `dfrac{3}{7}`, `dfrac{1}{2}`, `dfrac{4}{7}`, `dfrac{3}{5}`, `dfrac{5}{8}`, `dfrac{2}{3}`, `dfrac{5}{7}`, `dfrac{3}{4}`, `dfrac{4}{5}`, `dfrac{5}{6}`, `dfrac{6}{7}`, `dfrac{7}{8}`
//
// It can be seen that `dfrac{2}{5}` is the fraction immediately to the left of `dfrac{3}{7}`.
//
// By listing the set of reduced proper fractions for `d <= 1,000,000` in ascending order of size, find the numerator of the fraction immediately to the left of `dfrac{3}{7}`.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// While researching several approaches I read something about Farey sequences (see https://en.wikipedia.org/wiki/Farey_sequence):
// If you know two neighboring fractions `dfrac{a}{b}` and `dfrac{c}{d}`, then there exists a so-called mediant `m=dfrac{a+c}{b+d}`.
//
// `dfrac{0}{1}` and `dfrac{1}{1}` are neighboring fractions because the fulfil the condition `bc - ad = 1`.
//
// My program starts with these two fractions. In each iteration it checks whether the mediant is less or greater than the user input (`3/7` by default).
// - `m < 3/7` ==> continue searching in interval `a/b` and `m`
// - `m < 3/7` ==> continue searching in interval `m` and `c/d`
// - if denominator of `m` exceeds 1000000 then stop
//
// The first steps will be:
// interval `0/1` and `1/1` => `(0+1)/(1+1)` = `1/2` which is larger  than `3/7`
// interval `0/1` and `1/2` => `(0+1)/(1+2)` = `1/3` which is smaller than `3/7`
// interval `1/3` and `1/2` => `(1+1)/(3+2)` = `2/5` which is smaller than `3/7`
// interval `2/5` and `1/2` => `(2+1)/(5+2)` = `3/7` which is equal   to `3/7` (treat the same as if larger)
// interval `2/5` and `3/7` => `(2+3)/(5+7)` = `7/12` ... and so on ...
//
// The algorithm finds the correct result but might be too slow.
// Therefore, I stop whenever I my right interval border becomes `a/b` (which is `3/7` in the example) because after that only the left interval border can change.
// Then a much faster explicit computation is possible:
// - each iteration would adjust the left border: ''leftN += rightN'' and ''leftD += rightD'' until ''leftN + leftD > limit''.
// - there would be ''1 + (limit - (leftD + rightD)) / rightD'' iterations
//
// I wrote a function ''isLess'' that compares two fractions and returns ''true'' if `dfrac{a}{b} < dfrac{c}{d}`:
// if `a/b < c/d` (and all numbers are positive) then `ad < cb`
// This can be done with precise integer arithmetic. However, Hackerrank's denominators are up to `10^15` and therefore the product exceeds 64 bits.
// The template ''multiply'' is a portable way to multiply two 64 bits and return the 128 bit result as two 64 bit variables.
// Note: GCC supports ''__int128'' and there is no need to have such a template on Linux/Apple/MinGW.

#include <iostream>

// multiply two unsigned number where the result may exceed the largest native data type
template <typename T>
void multiply(T a, T b, T& result_high, T& result_low)
{
  const T Shift = 4 * sizeof(a);
  const T Mask  = T(~0) >> Shift;

  auto a_high = a >> Shift;
  auto a_low  = a &  Mask;
  auto b_high = b >> Shift;
  auto b_low  = b &  Mask;

  auto c_0  = a_low  * b_low;
  auto c_1a = a_high * b_low;
  auto c_1b = a_low  * b_high;
  auto c_2  = a_high * b_high;

  auto carry = ((c_0 >> Shift) + (c_1a & Mask) + (c_1b & Mask)) >> Shift;

  result_high = c_2 + (c_1a >> Shift) + (c_1b >> Shift) + carry;
  result_low  = c_0 + (c_1a << Shift) + (c_1b << Shift);
}

// if   a/b < c/d (and all numbers are positive)
// then a*d < c*b
bool isLess(unsigned long long a, unsigned long long b, unsigned long long c, unsigned long long d)
{
  // GCC has 128-bit support:
  //return (unsigned __int128)x1 * y2 < (unsigned __int128)y1 * x2;

  unsigned long long r1_high, r1_low;
  unsigned long long r2_high, r2_low;
  multiply(a, d, r1_high, r1_low);
  multiply(c, b, r2_high, r2_low);
  // compare high bits
  if (r1_high < r2_high)
    return true;
  if (r1_high > r2_high)
    return false;
  // compare low bits
  return (r1_low < r2_low);
}

int main()
{
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int a = 3;
    unsigned int b = 7;
    unsigned long long limit = 1000000;
    std::cin >> a >> b >> limit;

    // generate all numbers in the Farey sequence using binary subdivision
    // for each step decide whether the number was left or right of the desired fraction
    // start with 0/1 and 1/1
    unsigned long long leftN  = 0;
    unsigned long long leftD  = 1;
    unsigned long long rightN = 1;
    unsigned long long rightD = 1;
    while (leftD + rightD <= limit)
    {
      auto mediantN = leftN + rightN;
      auto mediantD = leftD + rightD;

      // if   x1/y1 < x2/y2 (and all numbers are positive)
      // then x1*y2 < x2*y1
      if (isLess(mediantN, mediantD, a, b))
      {
        // adjust left border
        leftN = mediantN;
        leftD = mediantD;
      }
      else
      {
        // adjust right border
        rightN = mediantN;
        rightD = mediantD;
        // did we "hit the spot" ?
        if (rightN == a && rightD == b)
          break;
      }
    }

    // now the right border is the fraction we read from input
    // and we only have to adjust the left border from here on

    //while (leftD + rightD <= limit) // gets the job done, but still slow ...
    //{
    //  leftN += rightN;
    //  leftD += rightD;
    //}

    // "instant" result
    if (limit >= leftD + rightD)
    {
      auto difference = limit - (leftD + rightD);
      auto repeat = 1 + difference / rightD;
      leftN += repeat * rightN;
      leftD += repeat * rightD;
    }

    std::cout << leftN << " " << leftD << std::endl;
  }
  return 0;
}
