// ////////////////////////////////////////////////////////
// # Title
// Red, green or blue tiles
//
// # URL
// https://projecteuler.net/problem=116
// http://euler.stephan-brumme.com/116/
//
// # Problem
// A row of five black square tiles is to have a number of its tiles replaced with coloured oblong tiles chosen from red (length two), green (length three), or blue (length four).
//
// If red tiles are chosen there are exactly seven ways this can be done.
//
// ![red tiles](red.gif)
//
// If green tiles are chosen there are three ways.
//
// ![green tiles](green.gif)
//
// And if blue tiles are chosen there are two ways.
//
// ![blue tiles](blue.gif)
//
// Assuming that colours cannot be mixed there are 7 + 3 + 2 = 12 ways of replacing the black tiles in a row measuring five units in length.
//
// How many different ways can the black tiles in a row measuring fifty units in length be replaced if colours cannot be mixed and at least one coloured tile must be used?
//
// NOTE: This is related to Problem 117.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// The row is filling with an arbitrary sequence of black and colored blocks.
// The length of the sequence is the sum of black and colored blocks.
// For a known number of black and colored blocks, this is:
// `{black+colored}choose{black}` `= dfrac{(black+colored)!}{black!colored!}`
//
// It took me a little bit to write a ''choose'' function such that the factorials don't overflow too soon.
//
// # Hackerrank
// As always, Hackerrank's input is ridiculously high and my poor C++ data types can't handle such numbers.
// [TODO] find closed formula

#include <iostream>
#include <vector>

#define ORIGINAL

// number of ways to choose n elements from k available
unsigned long long choose(unsigned long long n, unsigned long long k)
{
  // n! / (n-k)!k!
  unsigned long long result = 1;
  // reduce overflow by dividing as soon as possible to keep numbers small
  for (unsigned long long invK = 1; invK <= k; invK++)
  {
    result *= n;
    result /= invK;
    n--;
  }
  return result;
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // length of row
    unsigned int totalLength = 50;
    std::cin >> totalLength;

    unsigned long long sum = 0;
    // try all four block lengths
    for (unsigned int blockLength = 2; blockLength <= 4; blockLength++)
    {
      // maximum number of blocks of the current size
      auto maxBlocks = totalLength / blockLength;

      // insert 1 to maxBlocks colored blocks
      for (unsigned int colored = 1; colored <= maxBlocks; colored++)
      {
        // remaining black tiles
        auto black = totalLength - colored * blockLength;
        // total number of black and colored tiles
        auto tiles = black + colored;
        // count combinations
        auto combinations = choose(tiles, colored);
        sum += combinations;

#ifndef ORIGINAL
        // Hackerrank only
        sum %= 1000000007;
#endif
      }
    }

    std::cout << sum << std::endl;
  }

  return 0;
}
