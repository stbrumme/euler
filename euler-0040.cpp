// ////////////////////////////////////////////////////////
// # Title
// Champernowne's constant
//
// # URL
// https://projecteuler.net/problem=40
// http://euler.stephan-brumme.com/40/
//
// # Problem
// An irrational decimal fraction is created by concatenating the positive integers:
//
// `0.12345678910\red{1}112131415161718192021...`
//
// It can be seen that the 12th digit of the fractional part is 1.
// If `d_n` represents the nth digit of the fractional part, find the value of the following expression.
//
// `d_1 * d_10 * d_100 * d_1000 * d_10000 * d_100000 * d_1000000`
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The original problem can be solved in a trivial way:
// - a ''for''-loop appends numbers to a long string until that string contains enough digits
// - read relevant digits, convert them from ASCII to integers and multiply them
//
// The Hackerrank problem asks for digits at positions up to `2^18` which cannot be done the brute force way
// because we would be running out of memory (and CPU time).
//
// My function ''getDigit'' finds a digit without building such a long string.
// It is based on the following observation:
// - there are 9 numbers with one digit (1 to 9)
// - there are 90 numbers with one digit (10 to 99)
// - there are 900 numbers with one digit (100 to 999)
// - ... and so on
//
// The first part of ''getDigit'' figures out how many digits the number has which is pointed to by the parameter ''pos''.
// - the first 9 numbers are represented by 1*9 digits in Champernowne's constant
// - the next 90 numbers are represented by 2*90=180 digits
// - the next 900 numbers are represented by 3*900=2700 digits
// - ... and so on: ''range'' will be 9, 90, 900, ... and ''digits'' will be 1, 2, 3, ...
//
// The variable ''skip'' will contain 9, 9+2*90 = 189, 9+2*90+3*900 = 2890 until the next step would exceed ''pos''.
// Now that the function knows how many digit the number (pointed to by ''pos'') has, ''getDigit'' process its digits.
//
// To do so, it moves ''first'' closer to ''pos'' by repeated adding ''range''.
// Whenever ''range'' becomes too large, the next (smaller) digit has to be adjusted until we have the final value of ''first''.
// That number is converted to a string and the desired digit returned.

#include <string>
#include <iostream>

// return the digit at position "pos"
// first digit after then decimal dot has pos = 1 (not zero !)
unsigned int getDigit(unsigned long long pos)
{
  // assume pos has one digit
  unsigned int       digits = 1;
  // then there are 9 other numbers
  unsigned long long range  = 9;
  // the smallest of them is 1
  unsigned long long first  = 1;

  // there are    9 numbers with 1 digit
  // there are   90 numbers with 2 digits
  // there are  900 numbers with 3 digits
  // there are 9000 numbers with 4 digits
  // ...
  // let's figure out the number of digits

  // skip numbers with too few digits
  unsigned long long skip = 0;
  while (skip + digits*range < pos)
  {
    skip += digits*range;
    // digits = 2 => range = 90 and
    // digits = 3 => range = 900
    // digits = 4 => range = 9000, etc.
    digits++;
    range *= 10;
    first *= 10;
  }

  // now that we know the number of digits
  // adjust "first" and "skip" such that the left-most/highest digit of pos and skip are identical
  // then continue with the next digit
  while (range > 9)
  {
    // could be replace by some modular arithmetic, but I'm too lazy for tough thinking ;-)
    while (skip + digits*range < pos)
    {
      skip  += digits*range;
      first += range;
    }

    // next lower digit
    range /= 10;
  }

  // right-most digit (basically same inner loop as above when range == 1)
  while (skip + digits < pos)
  {
    first++;
    skip += digits;
  }

  // skip all "skippable" digits
  pos -= skip;
  // strings are zero-based whereas input is one-based
  pos--;

  // create a string version of our number
  auto s = std::to_string(first);
  // extract digit and convert from ASCII to an integer
  return s[pos] - '0';
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int product = 1;
    // read 7 positions
    for (unsigned int i = 0; i < 7; i++)
    {
      unsigned long long pos;
      std::cin >> pos;

      // multiply all digits
      product *= getDigit(pos);
    }

    // print result
    std::cout << product << std::endl;
  }

  return 0;
}
