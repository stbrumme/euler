// ////////////////////////////////////////////////////////
// # Title
// Reciprocal cycles
//
// # URL
// https://projecteuler.net/problem=26
// http://euler.stephan-brumme.com/26/
//
// # Problem
// A unit fraction contains 1 in the numerator. The decimal representation of the unit fractions with denominators 2 to 10 are given:
//
// `frac{1}{2} = 0.5`
// `frac{1}{3} = 0.overline{3}`
// `frac{1}{4} = 0.25`
// `frac{1}{5} = 0.2`
// `frac{1}{6} = 0.1\overline{6}`
// `frac{1}{7} = 0.overline{142857}`
// `frac{1}{8} = 0.125`
// `frac{1}{9} = 0.overline{1}`
// `frac{1}{10} = 0.1`
//
// Where `0.1\overline{6}` means `0.166666...`, and has a 1-digit recurring cycle.
// It can be seen that `frac{1}{7}` has a 6-digit recurring cycle.
//
// Find the value of `d < 1000` for which `1/d` contains the longest recurring cycle in its decimal fraction part.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// I implemented the same basic division algorithm I learnt in school (3rd grade ?!)
// but of course extended it to operate with fractionals instead of integers, e.g. `1/7 = 0.overline{142857}`
//
// Let's do it the good old-fashioned way:
// 1    / 7 = 0 (and 1 remains, multiply it by 10 and it becomes our next dividend)
// 1*10 / 7 = 1 (and 3 remains, multiply as above)
// 3*10 / 7 = 4 (and 2 remains, you know the game)
// 2*10 / 7 = 2 (and 6 remains)
// 6*10 / 7 = 8 (and 4 remains)
// 4*10 / 7 = 5 (and 5 remains)
// 5*10 / 7 = 7 (and 1 remains)
//
// Now we have the same remainder 1 we had in the first step and the circle begins again.
// Moreover, the digits after the equation sign produce the recurring cycle 142857.
// Their length is 6 because it took six steps until we saw the same remainder again.
//
// More or less the same algorithm can be found in the Wikipedia, too: https://en.wikipedia.org/wiki/Repeating_decimal
//
// Similar to many problems before, the modified Hackerrank problem forced me to precompute the results
// and then just look up those numbers for each test case.

#include <iostream>
#include <vector>

// find length of the recurring cycle in the decimal fraction part of 1/x
unsigned int cycleLength(unsigned int x)
{
  // catch invalid fractions
  if (x == 0)
    return 0;

  // [remainder] => [pos]
  const unsigned int NotSeenYet = 0;
  std::vector<unsigned int> lastPos(x, NotSeenYet);

  // start at first digit after the decimal dot
  unsigned int position = 1;
  // 1/x => initial dividend is 1
  unsigned int dividend = 1;

  // exit-conditions are inside the loop
  while (true)
  {
    // find remainder
    unsigned int remainder = dividend % x;

    // if remainder becomes zero then stop immediately
    // because the fraction doesn't have a recurring cycle
    if (remainder == 0)
      return 0;

    // same remainder ? => abort
    if (lastPos[remainder] != NotSeenYet)
      // length of recurring cycle
      return position - lastPos[remainder];

    // remember position of current remainder
    lastPos[remainder] = position;

    // next step
    position++;
    dividend = remainder * 10;
  }
}

int main()
{
  // Hackerrank's upper limit
  const unsigned int MaxDenominator = 10000;

  // cache results to speed up running tons of test cases
  std::vector<unsigned int> cache = { 0 }; // no cycles for 1/0

  unsigned int longestDenominator = 0;
  unsigned int longestCycle       = 0;
  for (unsigned int denominator = 1; denominator <= MaxDenominator; denominator++)
  {
    // found a longer circle ?
    auto length = cycleLength(denominator);
    if (longestCycle < length)
    {
      longestCycle       = length;
      longestDenominator = denominator;
    }

    // cache result
    cache.push_back(longestDenominator);
  }

  // plain look up
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int x;
    std::cin >> x;
    // find "best" denominator smaller (!) than the input value, therefore minus one
    std::cout << cache[x - 1] << std::endl;
  }
  return 0;
}
