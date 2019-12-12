// ////////////////////////////////////////////////////////
// # Title
// Divisor Square Sum
//
// # URL
// https://projecteuler.net/problem=211
// http://euler.stephan-brumme.com/211/
//
// # Problem
// For a positive integer `n`, let `\sigma(n)` be the sum of the squares of its divisors. For example,
// `\sigma^2(10) = 1 + 4 + 25 + 100 = 130`.
//
// Find the sum of all `n`, `0 < n < 64,000,000` such that `\sigma^2(n)` is a perfect square.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// My approach works like a sieve:
// - allocate enough memory for 64000000 numbers, each 64 bit (=> 512 MByte)
// - fill it with zeros
// - for each number 0 < i < 64000000: add i*i to each cell that is a multiple of i
// - when done, check each cell whether it is a perfect square
//
// The initial solution was small and produced the correct result in about 10 seconds.
// However, its memory consumption was much, much higher than any of my other solutions.
//
// Therefore I decided to move the algorithm to a new function ''processSlice'' which works a bit smarter:
// instead of processing everything at once, it only looks at all numbers [''from, to''].
// Reducing ''to - from'' to about 4 million doesn't slow down the algorithm at all.
// But I wanted to keep it below 20 MByte (for no good reason ...) and chose a ''sliceSize = 2000000'' which takes about 14 seconds (40% slower).
//
// # Alternative
// You can play around with prime factorization. This should be a bit faster at the cost of probably doubling the code size.

#include <iostream>
#include <vector>
#include <cmath>

// determine the sum of all numbers between "from" and "to" (inclusive both) which match the problem statement
unsigned long long processSlice(unsigned int from, unsigned int to)
{
  std::vector<unsigned long long> sumSquares(to - from + 1, 0);
  // like a prime sieve: add square of all divisors
  for (unsigned long long i = 1; i <= to; i++)
  {
    // position of smallest multiple of i >= from
    auto pos = (from / i) * i;
    if (pos < from)
      pos += i;

    // add i^2 to all multiples of i
    for (; pos <= to; pos += i)
      sumSquares[pos - from] += i*i;
  }

  // find all sums that are perfect squares
  unsigned int sum = 0;
  for (size_t i = 0; i < sumSquares.size(); i++)
  {
    auto number  = i + from;
    auto current = sumSquares[i];

    // compute integer square root
    unsigned long long root = sqrt(current);
    // iff root^2 = current then it's a perfect square
    if (root * root == current)
      sum += number;
  }

  return sum;
}

int main()
{
  unsigned int limit = 64000000;
  std::cin >> limit;

  // how many number should be analyzed at once (=> influences memory consumption)
  unsigned int sliceSize = 2000000;

  // total sum
  unsigned int sum  = 0;
  // start of current slice
  unsigned int from = 1;
  while (from < limit)
  {
    // end of current slice
    auto to = from + sliceSize - 1;
    if (to >= limit)
      to = limit;

    // process current slice
    sum += processSlice(from, to);
    // next slice
    from = to + 1;
  }

  // print result
  std::cout << sum << std::endl;
  return 0;
}
