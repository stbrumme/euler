// ////////////////////////////////////////////////////////
// # Title
// Consecutive positive divisors
//
// # URL
// https://projecteuler.net/problem=179
// http://euler.stephan-brumme.com/179/
//
// # Problem
// Find the number of integers `1 < n < 10^7`, for which `n` and `n + 1` have the same number of positive divisors.
// For example, 14 has the positive divisors 1, 2, 7, 14 while 15 has 1, 3, 5, 15.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// Finally a simple problem ... I create an array ''divisors'' with `10^7` entries.
// Two nested loops go through all pairs `(i,k)` where `i * k < 10^7` and increment each entry at ''divisors[i * k]'' (in my code ''j = i * k'').
//
// A second pass counts how often ''divisors[n] == divisors[n + 1]''.
//
// # Note
// 8648640 has the most divisors: 447.
//
// A ''short'' uses less memory than an ''int'' which caused less memory stalls (while still being able to store that maximum value of 447).
// I saw a 20% performance boost on my system when switching from ''int'' to ''short''.
//
// Each number is divisible by 1 and by itself. When excluding those two divisors I will still find the correct solution.
// However, the program didn't become faster when starting the outer loop at ''2'' (instead of ''1'') and the inner loop at ''2*i'' (instead of ''i'').

#include <iostream>
#include <vector>

#define ORIGINAL

int main()
{
  // almost like a reverse prime sieve ...
  unsigned int limit = 10000000;
#ifdef ORIGINAL
  std::cin >> limit;
#endif

  // count divisors of the number immediately after "limit", too
  limit++;

  // will have the number of divisors for each number
  std::vector<unsigned short> divisors(limit, 0);

  // all numbers which can be a divisor ...
  for (unsigned int i = 1; i <= limit / 2; i++)
    // and all of their multiples
    for (unsigned int j = i; j <= limit; j += i)
      divisors[j]++;

#ifdef ORIGINAL
  unsigned int result = 0;
  for (unsigned int i = 2; i < limit; i++)
    if (divisors[i] == divisors[i + 1])
      result++;
  std::cout << result << std::endl;
#else
  // [index] => [numbers up to index which match the "neighbor" condition]
  std::vector<unsigned int> count(limit + 1, 0);
  // count numbers whose bigger neighbors has the same number of divisors
  for (unsigned int i = 2; i < limit; i++)
  {
    count[i] = count[i - 1];
    if (divisors[i] == divisors[i + 1])
      count[i]++;
  }

  // simple lookup of results
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int index;
    std::cin >> index;
    std::cout << count[index - 1] << std::endl;
  }
#endif

  return 0;
}
