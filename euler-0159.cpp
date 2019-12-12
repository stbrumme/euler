// ////////////////////////////////////////////////////////
// # Title
// Digital root sums of factorisations
//
// # URL
// https://projecteuler.net/problem=159
// http://euler.stephan-brumme.com/159/
//
// # Problem
// A composite number can be factored many different ways. For instance, not including multiplication by one, 24 can be factored in 7 distinct ways:
// `24 = 2 * 2 * 2 * 3`
// `24 = 2 * 3 * 4`
// `24 = 2 * 2 * 6`
// `24 = 4 * 6`
// `24 = 3 * 8`
// `24 = 2 * 12`
// `24 = 24`
//
// Recall that the digital root of a number, in base 10, is found by adding together the digits of that number,
// and repeating that process until a number is arrived at that is less than 10. Thus the digital root of 467 is 8.
//
// We shall call a Digital Root Sum (DRS) the sum of the digital roots of the individual factors of our number.
// The chart below demonstrates all of the DRS values for 24.
//
// ||       7       ||       10         ||
// ||!Factorisation ++ Digital Root Sum ||
// ||    2x2x2x3    ++        9         ||
// ||     2x3x4     ++        9         ||
// ||     2x2x6     ++       10         ||
// ||      4x6      ++       10         ||
// ||      3x8      ++       11         ||
// ||     2x12      ++        5         ||
// ||       24      ++        6         ||
//
// The maximum Digital Root Sum of 24 is 11.
// The function `mdrs(n)` gives the maximum Digital Root Sum of `n`. So `mdrs(24)=11`.
// Find `sum{mdrs(n)}` for `1 < n < 1000000`.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// The problem statement explains that `mdrs(a * b) = mdrs(a) + mdrs(b)`.
//
// I needed a bit to realize that if `x = a * b * c` then `mdrs(a * b * c) = max(mdrs(a) + mdrs(b * c), mdrs(a * b) + mdrs(c))`
//
// That means that `mdrs(2 * 2 * 6) = max(mdrs(2) + mdrs(12), mdrs(4) + mdrs(6)) = max(5, 10) = 10`.
//
// Right at the start, my program fills the ''mdrs'' container with the "non-factorized" ''digitRoot(x)'' (same as the last line in the problem statement's table).
//
// Then for each `a` I find all its multiples `a * b` and update `mdrs(a * b)` if `mdrs(a * b) < mdrs(a) + mdrs(b)`.
//
// The sum of all relevant `mdrs` is a simple loop.
//
// # Note
// After I finished my program I read the Wikipedia page https://en.wikipedia.org/wiki/Digital_root and saw that a much simpler, faster,
// non-iterative and/or recursive formula for ''digitRoot(x)'' is possible: ''result = ((x - 1) % 9) + 1''
//
// # Hackerrank
// The program must be able to handle tons of test cases. That's why I precompute all results in step 1 and then simply look them up in step 2.
// And there's a tiny difference in the way the upper limit is handled: the original problem excludes it while Hackerrank includes it.

#include <iostream>
#include <vector>
#include <algorithm>

//#define ORIGINAL

// find digit root of a number
unsigned int digitRoot(unsigned int x)
{
  // taken from https://en.wikipedia.org/wiki/Digital_root
  //return ((x - 1) % 9) + 1;

  // and my original code:
  unsigned int result = 0;
  while (x > 0)
  {
    result += x % 10;
    x      /= 10;
  }

  // repeat again ?
  if (result >= 10)
    result = digitRoot(result);

  return result;
}

int main()
{
  unsigned int limit = 1000000;

#ifndef ORIGINAL
  // read all test cases
  limit = 1;
  unsigned int tests = 1;
  std::cin >> tests;
  std::vector<unsigned int> input(tests);
  for (auto& x : input)
  {
    std::cin >> x;
    if (limit < x)
      limit = x;
  }
#endif

  // step 1: precompute sum(mdrs) for every x

  std::vector<unsigned char> mdrs(limit + 1, 0);

  // digit root without any factorization
  for (unsigned int i = 2; i <= limit; i++)
    mdrs[i] = digitRoot(i);

  // for each number a ...
  for (unsigned int a = 2; a <= limit; a++)
    // ... adjust all its multiples a*b
    for (unsigned int b = 2; a * b <= limit && b <= a; b++)
      // improved ?
      if (mdrs[a * b] < mdrs[a] + mdrs[b])
        mdrs[a * b] = mdrs[a] + mdrs[b];

  // step 2: display result(s)

#ifdef ORIGINAL
  // sum of mdrs(2..limit-1)
  unsigned int sum = 0;
  for (unsigned int i = 2; i < limit; i++)
    sum += mdrs[i];

  std::cout << sum << std::endl;
#else
  // sum of mdrs(2..i) for every 2 <= i <= limit
  // note: unlike the original problem, it is including limit
  std::vector<unsigned int> sums(mdrs.size(), 0);
  for (unsigned int i = 2; i < sums.size(); i++)
    sums[i] = sums[i-1] + mdrs[i];

  // display results
  for (auto x : input)
    std::cout << sums[x] << std::endl;
#endif

  return 0;
}
