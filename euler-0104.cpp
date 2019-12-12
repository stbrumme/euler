// ////////////////////////////////////////////////////////
// # Title
// Pandigital Fibonacci ends
//
// # URL
// https://projecteuler.net/problem=104
// http://euler.stephan-brumme.com/104/
//
// # Problem
// The Fibonacci sequence is defined by the recurrence relation:
//
// `F_n = F_{n-1} + F_{n-2}`, where `F_1 = 1` and `F_2 = 1`.
//
// It turns out that `F_541`, which contains 113 digits, is the first Fibonacci number for which the last nine digits are 1-9 pandigital (contain all the digits 1 to 9, but not necessarily in order).
// And `F_2749`, which contains 575 digits, is the first Fibonacci number for which the first nine digits are 1-9 pandigital.
//
// Given that `F_k` is the first Fibonacci number for which the first nine digits AND the last nine digits are 1-9 pandigital, find `k`.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// My solution is based on a stripped down version of my ''BigNum'' class and stores 9 digits per cell, that's why it's called ''BillionNum'' (because ''MaxDigit = 1000000000'' ==> `10^9`).
// It only supports ''operator+=''.
//
// ''isPandigital(x, digits)'' returns true if ''x'' is ''1..digits''-pandigital, e.g. ''isPandigital(312, 3) == true'' because 312 is 3-pandigital.
// The original problem assumes ''digits = 9''.
// ''x'' 's digits are chopped off step-by-step and a bitmask tracks which digits we have already seen.
// Zero is not part of any pandigital number, not even implicit leading zeros.
//
// The ''main'' routines analyzes the lower digits first.
// Only when they are pandigital then the highest digits are checked, too, because that's bit slower:
// I take all digits from the highest cell of my ''BillionNum''. If there are too few digits, all digits in its neighboring cell are included.
// We might have too many digits now, therefore I remove the lowest digit until the number of digits is correct.
// If these digits are pandigital, then we are done.
//
// Finding the next Fibonacci number involves ''operator+='' of ''BillionNum''. The default algorithm is:
// `F_{next} = F_a + F_b`
// `F_a    = F_b`
// `F_b    = F_{next}`
// ==> quite a few memory allocations, and many object constructor/destruction will take place behind the curtain.
// A simple trick to improve performance is to use these equations instead, which get rid of these "memory/object bookkeeping" effects:
// `F_a   += F_b` (add in-place)
// `F_a  <=> F_b` (swap contents of both object, which is technically just swapping two pointers)
//
// The main performance boost comes from my next trick:
// - we check the lowest digit whether they are pandigital
// - we check the highest digit whether they are pandigital
// - __but we don't care what the other digits are__
// ==> whenever a number exceeds a certain size, I delete digits from the middle
// Currently I delete if there are more than 10 cells, which represent `9 x 10 = 90` digits.
// There is no special reason why I chose 10, it was the first number that I tried and it worked immediately.
// The solution is found 100x faster now ...
//
// # Alternatives
// I saw some people used clever `log` arithmetic to find the highest digits.
// This works for the original Fibonacci numbers but not the generalized Fibonacci numbers of the Hackerrank problem.
//
// # Hackerrank
// The user can define `F_1` and `F_2` as well as how many digits should be pandigital.

#include <iostream>
#include <vector>

//#define ORIGINAL

// store single digits with lowest digits first
// e.g. 1024 is stored as { 4,2,0,1 }
struct BillionNum : public std::vector<unsigned int>
{
  // must store exactly 9 digits per cell
  static const unsigned int MaxDigit = 1000000000;

  // fill one cell
  BillionNum(unsigned int x)
  {
    push_back(x);
  }

  // add a big number
  void operator+=(const BillionNum& other)
  {
    // this code is a 95% copy of my BigNum class

    // add in-place, make sure it's big enough
    if (size() < other.size())
      resize(other.size(), 0);

    unsigned int carry = 0;
    for (size_t i = 0; i < size(); i++)
    {
      // perform standard addition algorithm
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
        carry         = 0;
      }
      else
      {
        // yes, we have an overflow
        operator[](i) = carry - MaxDigit;
        carry         = 1;
      }
    }

    if (carry > 0)
      push_back(carry);
  }
};

// return true if x is pandigital (1..digits)
bool isPandigital(unsigned int x, unsigned int digits = 9)
{
  unsigned int mask = 0; // zero is not allowed, only 1 to 9
  for (unsigned int i = 0; i < digits; i++)
  {
    // get next digit
    auto current = x % 10;
    if (current == 0 || current > digits)
      return false;

    auto bit = 1 << current;
    // already seen ?
    if ((mask & bit) != 0)
      return false;

    // mark that digit as "used"
    mask |= bit;
    x    /= 10;
  }

  return true;
}

int main()
{
  unsigned int first  = 1; // F_1 = 1
  unsigned int second = 1; // F_2 = 1
  unsigned int digits = 9; // 9-pandigital (1..9)

#ifndef ORIGINAL
  std::cin >> first >> second >> digits;
#endif

  if (first == 1 && digits == 1)
  {
    std::cout << "1" << std::endl;
    return 0;
  }

  unsigned long long Modulo = 1; // 10^digits
  for (unsigned i = 1; i <= digits; i++)
    Modulo *= 10;

  // convert to a simplified BigNum
  BillionNum a = first;
  BillionNum b = second;

  for (unsigned int i = 2; i <= 2000000; i++)
  {
    // analyze the lowest digits
    auto lowest = b.front() % Modulo;
    bool isPanLow = isPandigital(lowest, digits);

    // look at the highest digits
    if (isPanLow)
    {
      unsigned long long highest = b.back();
      // maybe too few digits: use next cells, too
      if (highest < Modulo && b.size() > 1)
        highest = highest * BillionNum::MaxDigit + b[b.size() - 2];

      // too many digits ? shrink until we have the right number of digits
      while (highest >= Modulo)
        highest /= 10;

      // check if pandigital
      if (isPandigital(highest, digits))
      {
        // yes, pandigital on both ends !
        std::cout << i << std::endl;
        return 0;
      }
    }

    // next Fibonacci number
    a += b;
    std::swap(a, b);

    // don't compute all digits:
    // - look at the lowest digits
    // - and look at the highest (incl. some of its neighbors because of carry)
    // => remove those in the middle
    // => my "magic numbers" 10 and 2 were chosen pretty much at random ...
    if (a.size() > 10)
    {
      a.erase(a.begin() + 2);
      b.erase(b.begin() + 2);
    }
  }

  // failed
  std::cout << "no solution" << std::endl;
  return 0;
}
