// ////////////////////////////////////////////////////////
// # Title
// Hexadecimal numbers
//
// # URL
// https://projecteuler.net/problem=162
// http://euler.stephan-brumme.com/162/
//
// # Problem
// In the hexadecimal number system numbers are represented using 16 different digits:
// 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
//
// The hexadecimal number AF when written in the decimal number system equals 10*16+15=175.
//
// In the 3-digit hexadecimal numbers 10A, 1A0, A10, and A01 the digits 0,1 and A are all present.
// Like numbers written in base ten we write hexadecimal numbers without leading zeroes.
//
// How many hexadecimal numbers containing at most sixteen hexadecimal digits exist with all of the digits 0,1, and A present at least once?
// Give your answer as a hexadecimal number.
//
// (A,B,C,D,E and F in upper case, without any leading or trailing code that marks the number as hexadecimal and without leading zeroes,
// e.g. 1A3F and not: 1a3f and not 0x1a3f and not $1A3F and not #1A3F and not 0000001A3F)
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// A nice dynamic programming problem ... my function ''count'' has 5 parameters:
// - ''digits'' stands for the number of digits, initially 16
// - ''haveAny''  is only true if any of the preceding digits is different from zero
// - ''haveZero'' is only true if at least one of the preceding digits is ''0''
// - ''haveOne''  is only true if at least one of the preceding digits is ''1''
// - ''haveA''    is only true if at least one of the preceding digits is ''A''
//
// When looking at the current digit then there are four groups:
// 1. anything except ''0'', ''1'', ''A'':
// ==> that means 13x everything with ''digits - 1''
// ==> since it's not ''0'' we have at least one digit different from zero, i.e. ''haveAny'' must be true
//
// 2. current digit is ''0'':
// ==> if there was already a zero, it's the same like case 1
// ==> if there was no zero so far, then a zero is only allowed if ''haveAny'' is true (no leading zero !)
//
// 3. current digit is ''1'':
// ==> if there was already a one, it's the same like case 1
// ==> if there was no ''1'' so far, then set ''haveOne'' to true, ''haveAny'' as well
//
// 4. current digit is ''A'':
// ==> exactly the same thinking like case 3 but set ''haveA'' to true instead of ''haveOne''
//
// I abort early if all conditions ''haveZero'', ''haveOne'' and ''haveA'' are fulfilled.
//
// # Hackerrank
// All results have to be modulo `10^9+7` and up to 100 digits are allowed.
// The result should __not__ be displayed in hexadecimal.

//#define ORIGINAL

#include <iostream>
#include <iomanip>

// recursively count solutions
unsigned long long count(unsigned int digits, bool haveAny = false,
  bool haveZero = false, bool haveOne = false, bool haveA = false)
{
  // solved ?
  if (haveZero && haveOne && haveA && digits < 15)
    return 1ULL << (4 * digits); // same as pow(16, digits);

  // processed all digits ? (but no combination of 0, 1, A found)
  if (digits == 0)
    return 0;

  // assume current digit is not 0, 1 or A
  unsigned long long next = count(digits - 1, true, haveZero, haveOne, haveA);
  unsigned long long result = 13 * next;

  // try to use a zero (only allowed if already have any non-zero digit => "no leading zero")
  result += haveZero ? next : count(digits - 1, haveAny, haveAny, haveOne, haveA);
  // try to use 1
  result += haveOne  ? next : count(digits - 1, true, haveZero, true, haveA);
  // try to use A
  result += haveA    ? next : count(digits - 1, true, haveZero, haveOne, true);

#ifndef ORIGINAL
  result %= 1000000007ULL;
#endif

  return result;
}

int main()
{
#ifdef ORIGINAL
  std::cout << std::uppercase << std::hex << count(16) << std::endl;
#else
  unsigned int digits;
  std::cin >> digits;
  std::cout << count(digits) << std::endl;
#endif

  return 0;
}
