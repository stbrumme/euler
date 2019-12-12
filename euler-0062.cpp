// ////////////////////////////////////////////////////////
// # Title
// Cubic permutations
//
// # URL
// https://projecteuler.net/problem=62
// http://euler.stephan-brumme.com/62/
//
// # Problem
// The cube, 41063625 (`345^3`), can be permuted to produce two other cubes: 56623104 (`384^3`) and 66430125 (`405^3`).
// In fact, 41063625 is the smallest cube which has exactly three permutations of its digits which are also cube.
//
// Find the smallest cube for which exactly five permutations of its digits are cube.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// A function ''fingerprint'' was already used in problem 49 and problem 52: if ''fingerprint(a) == fingerprint(b)'' then ''a'' is a permutation of ''b''.
//
// Step 1 of my program computes all cubes from `1^3` to `maxCube^3`and stores them indexed by their fingerprint (see ''matches'').
//
// Step 2 goes through all fingerprints with the specified number of permutations and transfers each fingerprint's first cube (the "smallest") to an ''std::set'' called ''smallest''.
//
// Finally, ''smallest^3'' is printed - using the fact that each ''std::set'' is automatically sorted (needed for the Hackerrank version of the problem).
//
// # Hackerrank
// The modified problem requires all permutations to be below `maxCube^3`.
// When running the problem with the official solution (parameter ''xyz 5'' where ''xyz'' is the solution) then it will fail to find the correct solution.
//
// The reason is that all but the initial permutations of `maxCube^3` are bigger than `maxCube^3`.
// Therefore the solution is only found if all cubes are processed which produce the solution __and__ all bigger cubes that have still the same number of digits.
// To be safe, the parameter should be `solution * {^3}sqrt{10} approx solution * 2.16`.

#include <map>
#include <set>
#include <vector>
#include <iostream>

// count how often each digit occurs,
// store zero at lowest position, then ones, etc.
// e.g. 5063 means 3 zeros, 6 ones, no twos, 5 threes and nothing else
// note: can't handle input values with 2^6 or more identical digits
unsigned long long fingerprint(unsigned long long x)
{
  unsigned long long result = 0;
  while (x > 0)
  {
    // extract lowest digit
    auto digit = x % 10;
    x /= 10;

    // subdivide 64 bit integer into 10 "digit counters", each 6 bits wide
    // => each digit may occur up to 2^6=64 times, more than enough ...
    const auto BitsPerDigit = 6;
    result += 1ULL << (BitsPerDigit * digit);
  }
  return result;
}

int main()
{
  unsigned int maxCube = 10000;
  unsigned int numPermutations = 5;
  std::cin >> maxCube >> numPermutations;

  // [fingerprint] => [list of numbers, where number^3 produced that fingerprint]
  std::map<unsigned long long, std::vector<unsigned int>> matches;
  for (unsigned int i = 1; i < maxCube; i++)
  {
    // find fingerprint
    auto cube = (unsigned long long)i * i * i;
    // add current number to the fingerprint's list
    matches[fingerprint(cube)].push_back(i);
  }

  // extract all smallest cube, std::set is sorting them
  std::set<unsigned long long> smallest;
  for (auto m : matches)
    // right number of permutations ?
    if (m.second.size() == numPermutations)
      smallest.insert(m.second.front());

  // print in ascending order
  for (auto s : smallest)
    std::cout << (s*s*s) << std::endl;

  return 0;
}
