// ////////////////////////////////////////////////////////
// # Title
// Sum square difference
//
// # URL
// https://projecteuler.net/problem=6
// http://euler.stephan-brumme.com/6/
//
// # Problem
// The sum of the squares of the first ten natural numbers is,
// `1^2 + 2^2 + ... + 10^2 = 385`
//
// The square of the sum of the first ten natural numbers is,
// `(1 + 2 + ... + 10)^2 = 55^2 = 3025`
//
// Hence the difference between the sum of the squares of the first ten natural numbers
// and the square of the sum is `3025 - 385 = 2640`.
//
// Find the difference between the sum of the squares of the first one hundred natural numbers and the square of the sum.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// A very simple problem:
// - a ''for''-loop adds all natural numbers (''sum'') and their squares (''sumSquared'').
// - finally ''squaredSum=sum*sum''
// - and print the difference between ''squaredSum - sumSquared''
//
// The only minor hiccup was to switch from ''int'' to ''long long'' (basically from 32 to 64 bits).
//
// # Alternative
// The series of sums of all natural numbers are the so-called Triangular numbers ( https://en.wikipedia.org/wiki/Triangular_number ).
// They have a closed form, too:
// `sum{x}=frac{x(x+1)}{2}`
//
// And there is a closed form for the sum of squares as well ( https://en.wikipedia.org/wiki/Square_pyramidal_number )
// `sum{x^2}=frac{x(x+1)(2x+1)}{6}`
//
// I can easily derive the formula for triangular numbers but the one for the sum of squares isn't
// that obvious to me - I had to look it up.
//
// Therefore it would feel like cheating when using the closed form in my code without further explanation ...
// and the simple ''for''-loop is fast enough to pass all tests, too.

#include <iostream>

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long x;
    std::cin >> x;

    unsigned long long sum        = 0; // 1   + 2   + ...
    unsigned long long sumSquared = 0; // 1^2 + 2^2 + ...

    for (unsigned long long i = 1; i <= x; i++)
    {
      sum        += i;
      sumSquared += i*i;
    }
    // chances are that your compiler (partially) unrolls this simple loop

    // actually we don't need a loop for the sum (and the sum of squares)
    // => see "Alternative" section above

    // we had (1+2+...) instead of (1+2+...)^2
    unsigned long long squaredSum = sum * sum;
    std::cout << (squaredSum - sumSquared) << std::endl;
  }
  return 0;
}
