// ////////////////////////////////////////////////////////
// # Title
// Four Representations using Squares
//
// # URL
// https://projecteuler.net/problem=229
// http://euler.stephan-brumme.com/229/
//
// # Problem
// Consider the number 3600. It is very special, because
// `3600 = 48^2 +     36^2`
// `3600 = 20^2 + 2 * 40^2`
// `3600 = 30^2 + 3 * 30^2`
// `3600 = 45^2 + 7 * 15^2`
//
// Similarly, we find that `88201 = 99^2 + 280^2 = 287^2 + 2 * 54^2 = 283^2 + 3 * 52^2 = 197^2 + 7 * 84^2`.
//
// In 1747, Euler proved which numbers are representable as a sum of two squares.
// We are interested in the numbers `n` which admit representations of all of the following four types:
// `n = a_1^2 +   b_1^2`
// `n = a_2^2 + 2 b_2^2`
// `n = a_3^2 + 3 b_3^2`
// `n = a_7^2 + 7 b_7^2`,
// where the `a_k` and `b_k` are positive integers.
//
// There are 75373 such numbers that do not exceed `10^7`.
// How many such numbers are there that do not exceed `2 * 10^9`?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// My program works similar to a prime sieve:
// Two nested loops generate all ''a'' and ''b'' and then mark all `x = a^2 + k * b^2` (where `k \in { 1,2,3,7 }`).
// It's possible that there are multiple combinations of `a` and `b` for the same `k`.
// Therefore I use four bits because there are four different `k`.
//
// The naive approach needs 2 GByte RAM. A substantial part of my code is dedicated to process all `x` in a rolling fashion:
// Iterate over all `a` and then over all `b` but restrict `b` in such a way that `x = a^2 + k * b^2` is in a certain range.
// My current design processes 1 million `x` at once - which requires 1 MByte RAM and fits nicely in my CPU L2 cache.
// The four arrays ''b1'', ''b2'', ''b3'' and ''b7'' store the minimum ''b'' for each ''a'' for the current slice / range.
//
// # Alternative
// Some people came up with smart insights regarding special properties of the matching numbers modulo 168.
// Apparantly you can solve the problem in 1 second. Mine needs about 11 seconds.

#include <iostream>
#include <vector>
#include <cmath>

const unsigned int SliceSize = 1000*1000;

// bitmasks of for a^2 + k * b^2
const unsigned char One   = 1 << 0; // k = 1
const unsigned char Two   = 1 << 1; // k = 2
const unsigned char Three = 1 << 2; // k = 3
const unsigned char Seven = 1 << 3; // k = 7
// every matching number must have all four bits set
const unsigned char All   = One | Two | Three | Seven;

int main()
{
  unsigned int limit = 2*1000*1000*1000;
  std::cin >> limit;

  // how many numbers match the criteria
  unsigned int count = 0;

  // contains bitmasks for the current slice
  std::vector<unsigned char> used(SliceSize, 0);

  // store start values of b for each factor k=1,2,3,7
  unsigned int maxA = sqrt(limit);
  std::vector<unsigned int> b1(maxA + 1, 1);
  std::vector<unsigned int> b2(maxA + 1, 1);
  std::vector<unsigned int> b3(maxA + 1, 1);
  std::vector<unsigned int> b7(maxA + 1, 1);

  // split into slices
  unsigned int from = 0;
  while (from < limit)
  {
    // size of current slice
    unsigned int to = from + SliceSize;
    if (to > limit)
      to = limit;

    // process all a and b where a^2+kb^2 is within the current slice
    for (unsigned int a = 1; a*a + b1[a]*b1[a] < to; a++)
    {
      // a^2 + b^2
      unsigned int b = b1[a];
      for (; a*a +   b*b < to; b++)
        used[a*a +   b*b - from] |= One;
      b1[a] = b;

      // a^2 + 2b^2
      b = b2[a];
      for (; a*a + 2*b*b < to; b++)
        used[a*a + 2*b*b - from] |= Two;
      b2[a] = b;

      // a^2 + 3b^2
      b = b3[a];
      for (; a*a + 3*b*b < to; b++)
        used[a*a + 3*b*b - from] |= Three;
      b3[a] = b;

      // a^2 + 7b^2
      b = b7[a];
      for (; a*a + 7*b*b < to; b++)
        used[a*a + 7*b*b - from] |= Seven;
      b7[a] = b;
    }

    // count all matching numbers
    for (auto& x : used)
    {
      if (x == All)
        count++;

      // reset for next iteration
      x = 0;
    }

    from = to;
  }

  std::cout << count << std::endl;
  return 0;
}
