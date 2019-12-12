// ////////////////////////////////////////////////////////
// # Title
// Using up to one million tiles how many different "hollow" square laminae can be formed?
//
// # URL
// https://projecteuler.net/problem=173
// http://euler.stephan-brumme.com/173/
//
// # Problem
// We shall define a square lamina to be a square outline with a square "hole" so that the shape possesses vertical and horizontal symmetry.
// For example, using exactly thirty-two square tiles we can form two different square laminae:
//
// ![tiles](p173_square_laminas.gif)
//
// With one-hundred tiles, and not necessarily using all of the tiles at one time, it is possible to form forty-one different square laminae.
//
// Using up to one million tiles how many different square laminae can be formed?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// Each tiling consists of multiple "rings". The first example has two rings, the second only one.
// For each ring with side/edge length `x` we need `4(x-1)` tiles.
//
// My program iterates over all possible rings and tries to insert as many smaller rings as possible until one million is exceeded.
// The smallest ring has an edge length of 3 tiles. A ring inside another ring has an edge length which is 2 tiles shorter.
//
// # Hackerrank
// My brute-force approach solves the original problem in less than 0.01 seconds but can't solve situations with more than `10^9` tiles in a reasonable time.

#include <iostream>

int main()
{
  unsigned int limit = 1000000;
  std::cin >> limit;

  // result
  unsigned int count = 0;

  // start with smallest outer ring
  for (unsigned int outer = 3; ; outer++)
  {
    unsigned int sum = 0;
    // add as many inner rings as possible
    for (unsigned int inner = outer; inner >= 3; inner -= 2)
    {
      // tiles needed to create one ring whose sides are "inner" tiles long
      unsigned int ring = 4 * (inner - 1);

      // runnng out of tiles ?
      if (sum + ring > limit)
        break;

      // add valid ring
      sum += ring;
      count++;
    }

    // no more inner rings possible, abort
    if (sum == 0)
      break;
  }

  std::cout << count << std::endl;
  return 0;
}
