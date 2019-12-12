// ////////////////////////////////////////////////////////
// # Title
// Number Rotations
//
// # URL
// https://projecteuler.net/problem=168
// http://euler.stephan-brumme.com/168/
//
// # Problem
// Consider the number `142857`. We can right-rotate this number by moving the last digit (7) to the front of it, giving us `714285`.
// It can be verified that `714285 = 5 * 142857`.
// This demonstrates an unusual property of `142857`: it is a divisor of its right-rotation.
//
// Find the last 5 digits of the sum of all integers `n`, `10 < n < 10^100`, that have this property.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Let's analyze the example: `714285 = 5 * 142857`
// If I replace each digit by a letter then: `fabcde = x * abcdef` where `a=1`, `b=4`, `c=2`, `d=8`, `e=5`, `f=7` and `x=5`.
//
// There is a special relationship between those variables:
// `e = (x * f) mod 10 = (5 * 7) mod 10 = 35 mod 10 = 5`
//
// The first digit of `35` - which is `3` - influences the value of `d`:
// `d = (x * e + 3) mod 10 = (5 * 5 + 3) mod 10 = 28 mod 10 = 8`
//
// The first digit of `28` - which is `2` - influences the value of `c`:
// `c = (x * d + 2) mod 10 = (5 * 8 + 2) mod 10 = 42 mod 10 = 2`
//
// ... and so on.
// Just by knowing `x` and `f` I can create all other variables.
// However, not all produce a valid result: the product's first digit must be `f`, too.
// Leading zeros are disallowed as well.
//
// My program iterates for each number of digits ''numDigits'' over all possible values of `x` (parameter ''multiplier'') and `f` (parameter ''lastDigit'').
// If the generated sequence doesn't produce a first digit that matches `f` then it returns 0.
//
// The problem statement asks for the last five digits of all matching numbers, therefore a little ''modulo'' action is involved.
//
// # Note
// To me it wasn't clear whether ''multiplier'' can be ''1''. Because of this, many numbers with repeated digits are part of the solution
// (e.g. ''1111'', ''2222'', ''3333'', ...).

#include <iostream>

// return the unique n-digit number with a predefined last digit
// which after multiplication with a multiplier is a right-rotation of itself
// return only that value modulo some power-of-10
// if there is no such number, then return 0
unsigned int search(unsigned int numDigits, unsigned int multiplier, unsigned int lastDigit, unsigned int modulo)
{
  // will be 10, 100, 1000, 10000, ...
  unsigned int shift = 10;
  // first digit of last iteration
  unsigned int carry =  0;

  // set last digit
  unsigned int current = lastDigit;
  unsigned int result  = lastDigit;

  while (--numDigits)
  {
    // process next digit
    auto next = multiplier * current + carry;
    // "next" has at most two digits, carry over the first digit to next iteration
    carry     = next / 10;
    // and its second digit becomes part of "result"
    current   = next % 10;

    // current digit relevant for result ?
    if (shift < modulo)
    {
      result += current * shift;
      shift  *= 10;
    }
  }

  // left-most digit
  auto first = multiplier * current + carry;
  // failed ? (no leading zero, product's first digit must match last digit of factor)
  if (current == 0 || first != lastDigit)
    return 0;

  return result;
}

int main()
{
  unsigned int maxDigits = 100;
  std::cin >> maxDigits;

  // last five digits
  const unsigned int Modulo = 100000;

  unsigned int result = 0;
  // from 2 to 100 digits
  for (unsigned int numDigits = 2; numDigits <= maxDigits; numDigits++)
    // each multiplier between 1 and 9
    for (unsigned int multiplier = 1; multiplier <= 9; multiplier++)
      // last digit can't be zero, therefore 1 to 9 only
      for (unsigned int lastDigit = 1; lastDigit <= 9; lastDigit++)
        result += search(numDigits, multiplier, lastDigit, Modulo);

  std::cout << result % Modulo << std::endl;
  return 0;
}
