// ////////////////////////////////////////////////////////
// # Title
// Counting block combinations I
//
// # URL
// https://projecteuler.net/problem=114
// http://euler.stephan-brumme.com/114/
//
// # Problem
// A row measuring seven units in length has red blocks with a minimum length of three units placed on it,
// such that any two red blocks (which are allowed to be different lengths) are separated by at least one black square.
// There are exactly seventeen ways of doing this.
//
// ![pattern](pattern.gif)
//
// How many ways can a row measuring fifty units in length be filled?
//
// NOTE: Although the example above does not lend itself to the possibility, in general it is permitted to mix block sizes.
//       For example, on a row measuring eight units in length you could use red (3), black (1), and red (4).
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// This is a nice Dynamic Programming problem:
// - if there are less than three cells available then all must be black (1 solution)
// - else: the next cell  can be black (return ''count[space - 1]'')
// - or:   the next cells can be red, try all possible lengths and add ''count[space - block]''
//
// The algorithm examines most row lengths multiple times but memoizing the results in ''solutions'' keeps the running time well below 0.01 seconds.
//
// # Hackerrank
// My approach needs a bit of memory. Hackerrank has inputs up to `10^18` which clearly exceeds the RAM size of a desktop PC.
// [TODO] find closed formula

#include <iostream>
#include <vector>

#define ORIGINAL

// memoized solutions
const long long Unknown = -1;
std::vector<long long> solutions;

// print result modulo some number
#ifndef ORIGINAL
const unsigned long long Modulo = 1000000007;
#endif

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

  // Hackerrank only
#ifndef ORIGINAL
  result %= Modulo;
#endif

  // memoize result
  solutions[space] = result;
  return result;
}

int main()
{
  // minimum length of each red block
  unsigned int       minBlockLength =  3;
  // size of the whole row
  unsigned long long totalLength    = 50;
  std::cin >> totalLength >> minBlockLength;

  // cached results
  solutions.resize(totalLength + 1, Unknown);

  // let's go !
  std::cout << count(totalLength, minBlockLength) << std::endl;

  return 0;
}
