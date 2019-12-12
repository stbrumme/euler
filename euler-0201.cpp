// ////////////////////////////////////////////////////////
// # Title
// Subsets with a unique sum
//
// # URL
// https://projecteuler.net/problem=201
// http://euler.stephan-brumme.com/201/
//
// # Problem
// For any set `A` of numbers, let `sum(A)` be the sum of the elements of `A`.
// Consider the set `B = {1,3,6,8,10,11}`.
// There are 20 subsets of `B` containing three elements, and their sums are:
//
// `sum({1,3,6}) = 10`,
// `sum({1,3,8}) = 12`,
// `sum({1,3,10}) = 14`,
// `sum({1,3,11}) = 15`,
// `sum({1,6,8}) = 15`,
// `sum({1,6,10}) = 17`,
// `sum({1,6,11}) = 18`,
// `sum({1,8,10}) = 19`,
// `sum({1,8,11}) = 20`,
// `sum({1,10,11}) = 22`,
// `sum({3,6,8}) = 17`,
// `sum({3,6,10}) = 19`,
// `sum({3,6,11}) = 20`,
// `sum({3,8,10}) = 21`,
// `sum({3,8,11}) = 22`,
// `sum({3,10,11}) = 24`,
// `sum({6,8,10}) = 24`,
// `sum({6,8,11}) = 25`,
// `sum({6,10,11}) = 27`,
// `sum({8,10,11}) = 29`.
//
// Some of these sums occur more than once, others are unique.
// For a set `A`, let `U(A,k)` be the set of unique sums of `k`-element subsets of `A`, in our example we find `U(B,3) = {10,12,14,18,21,25,27,29}` and `sum(U(B,3)) = 156`.
//
// Now consider the 100-element set `S = {1^2, 2^2, ... , 100^2}`.
// `S` has 100891344545564193334812497256 50-element subsets.
//
// Determine the sum of all integers which are the sum of exactly one of the 50-element subsets of `S`, i.e. find `sum(U(S,50))`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Despite the high number of 50-elements subsets there is only a small number of possible sums:
// `sum_{i=50..100}{i^2} < 300000`
//
// That means I have to check about 300000 numbers whether they can be reached by adding 50 square numbers.
// To be more specific, I have to check whether there is exactly one way to reach them by adding 50 squares.
//
// The initial step is to generate all subsets of size 0 - there is only the empty set which is unique and has sum 0.
//
// Then I process all numbers `1^2`, `2^2`, ..., `100^2` and do the following:
// - iterate over all subset sizes, beginning with the highest (50)
// - pick each valid subset (''reachable[] = true'') and add the current number
// - check if the subset without the current number was unique (see ''duplicates[] = false'') and whether adding the current number produces a unique sum, too.
//
// Finally, add all unique sums of size 50.
//
// My first prototype was based on ''std::map'' but extremely slow and memory consuming.
// The published code works with highly efficient bit masks (''std::vector<bool>'').

#include <iostream>
#include <vector>

int main()
{
  unsigned int maxSet = 100;
  unsigned int choose =  50;
  std::cin >> maxSet >> choose;

  // all squares
  std::vector<unsigned int> values;
  for (unsigned int i = 1; i <= maxSet; i++)
    values.push_back(i*i);

  std::vector<std::vector<bool>> reachable (choose + 1);
  std::vector<std::vector<bool>> duplicates(choose + 1);
  for (size_t i = 0; i < reachable.size(); i++)
  {
    // heuristic to allocate only as much memory as needed
    size_t maxSize = maxSet*maxSet * i + 1;
    if (maxSize > 300000) // somewhat below 300000 different sums
      maxSize = 300000;
    reachable [i].resize(maxSize, false);
    duplicates[i].resize(maxSize, false);
  }

  // the empty set is always possible
  reachable[0][0] = true;

  // for each square ...
  for (auto add : values)
  {
    // try to append it to any valid set
    for (unsigned int numElements = choose; numElements > 0; numElements--)
    {
      // try every valid sum
      for (size_t sum = 0; sum < reachable[numElements - 1].size(); sum++)
      {
        // sum must be possible
        if (!reachable[numElements - 1][sum])
          continue;

        // append number to previous sum
        auto current = add + sum;

        // "sum" was a duplicate or "current" already exists ?
        if (duplicates[numElements - 1][sum] ||
            reachable[numElements][current])
        {
          // new sum is reachable but invalid
          reachable [numElements][current] = true;
          duplicates[numElements][current] = true;
        }
        else
        {
          // yes, new unique sum
          reachable[numElements][current] = true;
        }
      }
    }
  }

  // count all unique sums
  unsigned int result = 0;
  for (size_t sum = 0; sum < reachable[choose].size(); sum++)
    if (reachable[choose][sum] && !duplicates[choose][sum])
      result += sum;

  std::cout << result << std::endl;
  return 0;
}
