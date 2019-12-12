// ////////////////////////////////////////////////////////
// # Title
// Powerful digit counts
//
// # URL
// https://projecteuler.net/problem=63
// http://euler.stephan-brumme.com/63/
//
// # Problem
// The 5-digit number, `16807=7^5`, is also a fifth power. Similarly, the 9-digit number, `134217728=8^9`, is a ninth power.
//
// How many n-digit positive integers exist which are also an nth power?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// ''check(n)'' finds all such powers with ''n'' digits:
// - first it generates the smallest n-digit number ''from'' and the largest n-digit number ''to''
// - e.g. for ''n=5'' we have ''from=10000'' and ''to=99999''
// - then all numbers ''1^n'' to ''9^n'' are computed, if they are between ''from'' and ''to'' we have a match
//
// While experimenting I saw no number with more than 21 digits to fulfilled the problem's conditions.
//
// A minor headache was that 21 digits don't fit into C++ ''unsigned long long'' anymore.
// Therefore I switched to ''double'' which has a few rounding issues but they don't affect the original problem.
//
// # Hackerrank
// Hackerrank wants you to print all powers with a certain number of digits - instead of finding the number of all such powers.
// However, the rounding issues of ''double'' now come into play and the right-most digits are a bit off for large ''n''.
//
// Fortunately, Hackerrank wants us only to find the powers with at most 19 digits ... and a 64 bit ''unsigned long long'' is sufficient for this task.
// That's why you find the ''#ifdef'' construct where the type of ''Number'' is defined.
//
// # Alternative
// It's easy to determine the number of digits using ''log10''.
// And as mentioned in the code, ''pow(a,b)'' returns `a^b`.
// Both functions are available in the C++ standard library.

#include <iostream>

//#define ORIGINAL

// find all numbers where x^digits has digits digits (I'm loving that comment ...)
unsigned int check(unsigned int digits)
{
  // numbe rof matches
  unsigned int count = 0;

  // unsigned long long isn't sufficient for the highest digits
  // a double has some rounding issues but they don't affect to result
#ifdef ORIGINAL
typedef double Number;
#else
typedef unsigned long long Number;
#endif

  // range of valid numbers
  // from = 10^(digits-1)
  // to   = 10^digits - 1
  Number to = 1;
  for (unsigned int i = 1; i <= digits; i++)
    to *= 10;
  Number from = to / 10;
  to--;

  // try all single-digit base numbers
  for (unsigned int base = 1; base <= 9; base++)
  {
    // compute power = base ^ digits
    Number power = base;
    for (unsigned int i = 1; i < digits && power <= to; i++)
      power *= base;
    // could use C++'s pow(), too

    // right number of digits ?
    if (power >= from && power <= to)
    {
      count++;
#ifndef ORIGINAL
      std::cout << std::fixed << power << std::endl;
#endif
    }
  }

  return count;
}

int main()
{
#ifdef ORIGINAL
  // check all digits
  unsigned int count = 0;
  for (unsigned int digits = 1; digits <= 21; digits++) // I observed no results with more than 21 digits
    count += check(digits);
  std::cout << count << std::endl;

#else

  // check only certain digits
  unsigned int digits = 9;
  std::cin >> digits;
  check(digits);
#endif

  return 0;
}
