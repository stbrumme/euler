// ////////////////////////////////////////////////////////
// # Title
// Investigating in how many ways objects of two different colours can be grouped
//
// # URL
// https://projecteuler.net/problem=181
// http://euler.stephan-brumme.com/181/
//
// # Problem
// Having three black objects B and one white object W they can be grouped in 7 ways like this:
// (BBBW), (B,BBW), (B,B,BW), (B,B,B,W), (B,BB,W), (BBB,W), (BB,BW)
//
// In how many ways can sixty black objects B and forty white objects W be thus grouped?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// My solution is somehow similar to the coin-change algorithm:
// - each group can be treated as a sequence of black and white objects, BWBW is the same as BBWW
// - all groups can be sorted by their size and, if multiple groups have the same size, by their lexicographical order
//
// For ''maxBlack = 3'' and ''maxWhite = 1'' these groups exist:
// (B,B,B,W), (B,B,BW), (B,W,BB), (B, BBW), (W, BBB), (BB, BW), (BBBW)
// These are exactly the same as in the problem statement but in a different order.
//
// Two outer loops iterate over all possible sequences of black and white objects.
// The inner loops place them at every possible positions (until the number of available objects is exhausted).
//
// The result will be found in ''current[60][40]''.
//
// # Alternative
// You can solve this problem with Dynamic Programming, too.
// I wrote a simple prototype but it turned out to be much slower (8 seconds vs. 0.04 seconds).

#include <iostream>

// limits
const unsigned int MaxBlack = 160;
const unsigned int MaxWhite = 160;
const unsigned int Modulo   = 1000000007; // Hackerrank only

int main()
{
#define ORIGINAL
#ifndef ORIGINAL
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
#endif
  {
    unsigned int maxBlack = MaxBlack;
    unsigned int maxWhite = MaxWhite;
    std::cin >> maxBlack >> maxWhite;

    // static array size: actually it would be sufficient to use maxBlack instead of MaxBlack
    // (and maxWhite instead of MaxWhite)
    unsigned long long previous[MaxBlack + 1][MaxWhite + 1];
    unsigned long long current [MaxBlack + 1][MaxWhite + 1];

    // initialize
    for (unsigned int i = 0; i <= maxBlack; i++)
      for (unsigned int j = 0; j <= maxWhite; j++)
        previous[i][j] = 0;
    previous[0][0] = 1;

    // all possible subsets
    for (unsigned int useBlack = 0; useBlack <= maxBlack; useBlack++)
      for (unsigned int useWhite = 0; useWhite <= maxWhite; useWhite++)
      {
        // skip empty subset
        if (useBlack == 0 && useWhite == 0)
          continue;

        // put subset at every possible position
        for (unsigned int i = 0; i <= maxBlack; i++)
          for (unsigned int j = 0; j <= maxWhite; j++)
          {
            current[i][j] = 0;

            // place it repeatedly
            unsigned int k = 0;
            while (i >= k * useBlack && j >= k * useWhite)
            {
              current[i][j] += previous[i - k * useBlack][j - k *  useWhite];
              k++;
            }
#ifndef ORIGINAL
            current[i][j] %= Modulo;
#endif
          }

        // copy for next iteration
        for (unsigned int i = 0; i <= maxBlack; i++)
          for (unsigned int j = 0; j <= maxWhite; j++)
            previous[i][j] = current[i][j];
      }

    // print result
    std::cout << current[maxBlack][maxWhite] << std::endl;
  }

  return 0;
}
