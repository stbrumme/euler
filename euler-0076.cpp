// ////////////////////////////////////////////////////////
// # Title
// Counting summations
//
// # URL
// https://projecteuler.net/problem=76
// http://euler.stephan-brumme.com/76/
//
// # Problem
// It is possible to write five as a sum in exactly six different ways:
//
// 4 + 1
// 3 + 2
// 3 + 1 + 1
// 2 + 2 + 1
// 2 + 1 + 1 + 1
// 1 + 1 + 1 + 1 + 1
//
// How many different ways can one hundred be written as a sum of at least two positive integers?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Only very few adjustments to problem 31:
// - replace anything related to coins by the numbers 1..100
// - finally subtract 1 because the sum has to consist of at least two numbers (not just one)
// - for more details on the algorithm itself, please read my explanation of problem 31
//
// I am aware that there are more efficient solutions (even much shorter solutions !) but re-using old,
// proven code is still the fastest way to solve a problem ...

#include <iostream>
#include <vector>

typedef std::vector<unsigned long long> combinations;

int main()
{
  const unsigned int MaxNumber = 1000;
  // remember combinations for all combinations from 1 up to 1000
  std::vector<combinations> history;

  // store number of combinations in [x] if only summands up to x+1 are allowed:
  // [0] => combinations if only 1s are allowed
  // [1] => 1s and 2s are allowed, nothing more
  // [2] => 1s, 2s and 3s are allowed, nothing more
  // ...
  // [99] => all but 100 are allowed
  // [100] => using all numbers if possible

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    // number that should be represented as a sum
    unsigned int x = 100;
    std::cin >> x;

    // initially we start at zero
    // but if there are previous test cases then we can re-use the old results
    for (unsigned int j = history.size(); j <= x; j++)
    {
      combinations ways(MaxNumber, 0);

      // one combination if using only 1s
      ways[0] = 1;

      // use larger numbers, too
      for (unsigned int i = 1; i < MaxNumber; i++)
      {
        // first, pretend not to use that number
        ways[i] = ways[i - 1];

        // now use that number once (if possible)
        auto current = i + 1;
        if (j >= current)
        {
          auto remaining = j - current;
          ways[i] += history[remaining][i];
        }

        // only for Hackerrank
        // (it prevents printing huge numbers)
        ways[i] %= 1000000007;
      }

      // store information for future use
      history.push_back(ways);
    }

    // look up combinations
    auto result = history[x];
    // the last column contains the desired value
    auto combinations = result.back();

    // but it contains one undesired combination, too: the single number MaxNumber itself
    // (which fails to be "the sum of two (!) numbers", it's just one number)
    // therefore subtract 1
    combinations--;

    combinations %= 1000000007; // only for Hackerrank
    std::cout << combinations << std::endl;
  }
  return 0;
}
