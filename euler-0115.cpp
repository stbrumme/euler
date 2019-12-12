// ////////////////////////////////////////////////////////
// # Title
// Counting block combinations II
//
// # URL
// https://projecteuler.net/problem=115
// http://euler.stephan-brumme.com/115/
//
// # Problem
// NOTE: This is a more difficult version of Problem 114.
//
// A row measuring n units in length has red blocks with a minimum length of m units placed on it,
// such that any two red blocks (which are allowed to be different lengths) are separated by at least one black square.
//
// Let the fill-count function, F(m, n), represent the number of ways that a row can be filled.
//
// For example, F(3, 29) = 673135 and F(3, 30) = 1089155.
//
// That is, for m = 3, it can be seen that n = 30 is the smallest value for which the fill-count function first exceeds one million.
//
// In the same way, for m = 10, it can be verified that F(10, 56) = 880711 and F(10, 57) = 1148904, so n = 57 is the least value for which the fill-count function first exceeds one million.
//
// For m = 50, find the least value of n for which the fill-count function first exceeds one million.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// ... almost the same as problem 114 !
// Only the ''main'' function had to adjusted to find in a simple loop the smallest value to exceed one million.
//
// # Hackerrank
// My approach needs a bit of memory. Hackerrank has inputs up to `10^18` which clearly exceed the RAM size of a desktop PC.
// [TODO] find closed formula

#include <iostream>
#include <vector>

#define ORIGINAL

// memoized solutions
const long long Unknown = -1;
std::vector<long long> solutions;

// find result for row with a certain length
unsigned long long count(unsigned long long space, unsigned int minBlockLength)
{
  // finished ?
  if (space == 0)
    return 1;

  // already know the answer ?
  if (solutions[space] != Unknown)
    return solutions[space];

  // one option is to leave the next cell black
  auto result = count(space - 1, minBlockLength);
  // insert red blocks at the current position with all possible spaces
  for (auto block = minBlockLength; block <= space; block++)
  {
    // how much is left after inserting ?
    auto next = space - block;
    // must be followed by a black cell
    if (next > 0)
      next--;

    // count all combinations
    result += count(next, minBlockLength);
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
    // minimum length of each red block
    unsigned int       minBlockLength = 3;
    // what number should be exceeded ?
    unsigned long long limit          = 1000000;

    std::cin >> minBlockLength >> limit;

    // cached results
    solutions.clear();
    solutions.resize(limit + 1, Unknown); // not enough for Hackerrank

    for (unsigned int totalLength = 1; ; totalLength++)
    {
      auto combinations = count(totalLength, minBlockLength);
      if (combinations > limit)
      {
        std::cout << totalLength << std::endl;
        break;
      }
    }
  }

  return 0;
}
