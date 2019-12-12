// ////////////////////////////////////////////////////////
// # Title
// Special subset sums: meta-testing
//
// # URL
// https://projecteuler.net/problem=106
// http://euler.stephan-brumme.com/106/
//
// # Problem
// Let `S(A)` represent the sum of elements in set `A` of size `n`.
// We shall call it a special sum set if for any two non-empty disjoint subsets, `B` and `C`, the following properties are true:
//
// 1. `S(B) != S(C)`; that is, sums of subsets cannot be equal.
// 2. If `B` contains more elements than `C` then `S(B) > S(C)`.
//
// For this problem we shall assume that a given set contains n strictly increasing elements and it already satisfies the second rule.
//
// Surprisingly, out of the 25 possible subset pairs that can be obtained from a set for which `n = 4`, only 1 of these pairs need to be tested for equality (first rule).
// Similarly, when `n = 7`, only 70 out of the 966 subset pairs need to be tested.
//
// For `n = 12`, how many of the 261625 subset pairs that can be obtained need to be tested for equality?
//
// NOTE: This problem is related to Problem 103 and Problem 105.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Only subsets of the same size are candidates, the remaining combinations are disallowed by rule 2.
// I compare two subsets pair-wise, e.g. when looking at the subsets { 1,3 } and { 2,4 } I check whether 1<2 and 3<4.
// If for each of those pair the first number is smaller than the second number then the sum of the first set must be smaller than the sum of the second set
// and there is no need to check for equality (rule 1).
// Therefore I count how often that test fails, i.e. how often I find a number in the first subset that is larger than its partner in the second subset.
//
// I wrote a brute-force solution quite some time ago. It solves the original problem in less than 0.01 seconds but times out for every Hackerrank test case.
// Therefore I tried to find a better (= faster) algorithm. I read something about Catalan numbers ( https://en.wikipedia.org/wiki/Catalan_number )
// and Dyck paths (explained in the same Wikipedia artible) - and had the correct result in less time.
// However, my code was still definitely too slow for every Hackerrank test case.
// My ''choose'' function from problem 116 needs severe improvement and I'm currently out of ideas ...
//
// Nevertheless, here is my brute force solution (which still finishes in less than 0.01 seconds) where I used a few tricks from my website http://bits.stephan-brumme.com .
// The first step is to find all possible subsets:
// - generate all `2^12` subsets as bitmasks: if the n-th bit of a bitmask is set, then the corresponding subset contains the n-th element
// - store all those bitmasks/subsets where ''x'' bits are set in ''sets[x]''
//
// Then I compare each bitmask (''set1'') in ''sets[x]'' with all other bitmask (''set2'') in ''sets[x]'':
// - repeatedly extract the lowest set bit: ''set1'' 's lowest bit must be always at a lower position than the lowest bit of ''set2''
// - clear those lowest bits and continue until all bits are cleared
//
// # Hackerrank
// As mentioned before, my program is way too slow for Hackerrank: you are supposed to find the result for sets with up to
// `10^6` elements - and to make things worse, you have to do that for up to 30 sets within 2 seconds.

#include <iostream>
#include <vector>

// derived from http://bits.stephan-brumme.com/lowestBitSet.html
template <typename T>
T getLowestBit(T x)
{
  return ~x & (x - 1);
}
template <typename T>
void clearLowestBit(T& x)
{
  x &= x - 1;
}

int main()
{
  unsigned int size = 12;
  std::cin >> size;

  // create all subset of { 0,1,2,...,10,11 } as bitmasks
  // => if bit 2 is set then the subset contains 2

  // store in sets[x] all bitmasks where x bits are set (=> with x elements)
  std::vector<std::vector<unsigned int>> sets(size + 1);

  // create all 2^size sets (if size=12 ==> 4096 sets)
  auto numSets = 1 << size;
  for (auto i = 0; i < numSets; i++)
  {
    auto numBits = 0;
    auto reduce  = i;
    while (reduce != 0)
    {
      // clear lowest bit
      clearLowestBit(reduce);
      numBits++;
    }
    sets[numBits].push_back(i);
  }

  // count sets where equality has to be checked
  auto count = 0;

  // empty set and set with all elements don't to be checked (they are unique)
  // sets with only one element must be unique, too (problem description: "strictly increasing elements")
  for (unsigned int setSize = 2; setSize < size; setSize++)
  {
    auto setsSameSize = sets[setSize];

    // compare each set with each other (of the same set size)
    for (auto i = setsSameSize.begin(); i != setsSameSize.end(); i++)
      for (auto j = std::next(i); j != setsSameSize.end(); j++)
      {
        auto set1 = *i;
        auto set2 = *j;

        // make sure both sets are distinct (no common bits)
        if ((set1 & set2) != 0)
          continue;

        // set1 is always lexicographically smaller than set2,
        // therefore the n-th bit of set1 must be at lower position than the n-th bit of set2
        while (set1 != 0) // and set2 != 0
        {
          // extract lowest bit
          if (getLowestBit(set1) > getLowestBit(set2))
          {
            // mismatch, the n-th bit of set1 is at a high position
            // ==> that pair of sets needs a test for equality
            count++;
            break;
          }

          clearLowestBit(set1);
          clearLowestBit(set2);
        }
      }
  }

  // show result
  std::cout << count << std::endl;
  return 0;
}
