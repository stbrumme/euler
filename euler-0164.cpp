// ////////////////////////////////////////////////////////
// # Title
// Numbers for which no three consecutive digits have a sum greater than a given value
//
// # URL
// https://projecteuler.net/problem=164
// http://euler.stephan-brumme.com/164/
//
// # Problem
// How many 20 digit numbers n (without any leading zero) exist such that no three consecutive digits of n have a sum greater than 9?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// A straight-forward recursive solution:
// - start with an "empty" number (no digits at all)
// - append a digit (0 to 9) if the previous two digits and the current digits don't exceed 9
// - stop if enough digits where appended (''digits = 0'')
//
// A substantial number of calls will have the same parameter set. A small ''cache'' prevents re-computing the same results over and over again.
//
// # Hackerrank
// There can be up to 100 digits.
//
// # Note
// You can change the maximum sum of three consecutive digits, too (default is 9).

#include <iostream>

#define ORIGINAL

// Hackerrank: up to 100 digits
// Project Euler: 20 digits
const unsigned int MaxDigits = 100;

// bonus feature: change maximum sum of three consecutive digits
unsigned int maxSum = 9;

// count matching numbers
// prevprev: the digit two positions to the left of the current digit
// prev    : the digit one position  to the left of the current digit
// digits  : number of remainings digits
// isFirst : true only for the first digit (which must not be zero)
unsigned long long search(unsigned int prevprev, unsigned int prev, unsigned int digits, bool isFirst)
{
  // done ?
  if (digits == 0)
    return 1;

  // memoize
  unsigned int id = digits * 100 + prevprev * 10 + prev; // simple hash
  static unsigned long long cache[(MaxDigits + 1) * 100] = { 0 };
  if (cache[id] != 0)
    return cache[id];

  // iterate over all digits such that current + prev + prevprev <= 9
  unsigned long long result = 0;
  for (unsigned int current = 0; current + prev + prevprev <= maxSum; current++)
  {
    // no leading zero
    if (current == 0 && isFirst)
      continue;

    // add next digit
    result += search(prev, current, digits - 1, false);
  }

#ifndef ORIGINAL
  // Hackerrank only
  result %= 1000000007;
#endif

  // memoize
  cache[id] = result;
  return result;
}

int main()
{
  unsigned int digits = 20;
  std::cin >> digits;
  if (digits > MaxDigits)
    return 1;

#ifdef ORIGINAL
  // bonus feature: change maximum sum of three consecutive digits
  std::cin >> maxSum;
#endif

  std::cout << search(0, 0, digits, true) << std::endl;

  return 0;
}
