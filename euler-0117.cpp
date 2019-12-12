// ////////////////////////////////////////////////////////
// # Title
// Red, green, and blue tiles
//
// # URL
// https://projecteuler.net/problem=117
// http://euler.stephan-brumme.com/117/
//
// # Problem
// Using a combination of black square tiles and oblong tiles chosen from: red tiles measuring two units, green tiles measuring three units, and blue tiles measuring four units, it is possible to tile a row measuring five units in length in exactly fifteen different ways.
//
// ![tiles](pattern.gif)
//
// How many ways can a row measuring fifty units in length be tiled?
//
// NOTE: This is related to Problem 116.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// ... almost the same as problem 115 !
// Actually the code is even shorter - the ''for''-loop in ''count'' runs from 1 to 4 (1 = black, 2 = red, 3 = green, 4 = blue).
// There are no "gaps" between tiles because I treat black tiles the same way like any other tile.
//
// These numbers are also called "Generalized Fibonacci numbers", and the special case for this problem is called [Tetranacci numbers](https://en.wikipedia.org/wiki/Generalizations_of_Fibonacci_numbers#Tetranacci_numbers).
//
// # Hackerrank
// As always, Hackerrank's input is ridiculously high and my poor C++ data types can't handle such numbers.
// [TODO] find closed formula

#include <iostream>
#include <vector>

#define ORIGINAL

// memoized solutions
const long long Unknown = -1;
std::vector<long long> solutions;

// find result for row with a certain length
unsigned long long count(unsigned long long space)
{
  // finished ?
  if (space == 0)
    return 1;

  // already know the answer ?
  if (solutions[space] != Unknown)
    return solutions[space];

  // insert red blocks at the current position with all possible spaces
  unsigned long long result = 0;
  for (unsigned long long block = 1; block <= 4 && block <= space; block++)
  {
    // how much is left after inserting ?
    auto next = space - block;

    // count all combinations
    result += count(next);
  }

  // memoize result
  solutions[space] = result;
  return result;
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // what number should be exceeded ?
    unsigned long long limit = 50;
    std::cin >> limit;

    // cached results
    solutions.clear();
    solutions.resize(limit+1, Unknown);

    auto result = count(limit);
#ifndef ORIGINAL
    result %= 1000000007;                  // Hackerrank asks for "small" results
#endif

    std::cout << result << std::endl;
  }

  return 0;
}
