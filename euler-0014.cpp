// ////////////////////////////////////////////////////////
// # Title
// Longest Collatz sequence
//
// # URL
// https://projecteuler.net/problem=14
// http://euler.stephan-brumme.com/14/
//
// # Problem
// The following iterative sequence is defined for the set of positive integers:
//
// if `n` is even: `n \to n/2`
// if `n` is odd:  `n \to 3n + 1`
//
// Using the rule above and starting with 13, we generate the following sequence:
// `13 \to 40 \to 20 \to 10 \to 5 \to 16 \to 8 \to 4 \to 2 \to 1`
//
// It can be seen that this sequence (starting at 13 and finishing at 1) contains 10 terms.
// Although it has not been proved yet (Collatz Problem), it is thought that all starting numbers finish at 1.
//
// Which starting number, under one million, produces the longest chain?
//
// __NOTE:__ Once the chain starts the terms are allowed to go above one million.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The longest Collatz chain below five million contains 597 elements (and starts with 3732423).
// A brute-force algorithm solves this problem within a half a second.
//
// A smarter approach is to cache all chain lengths we encounter along the way.
// My function ''steps(x)'' tries to look up the result for ''x'' in a ''cache'':
// - if it succeeds, just return that cached value
// - if it fails, we do one step in the Collatz sequence and call ''step'' recursively
//
// The code runs approx. 10x faster now (but needs about 10 MByte RAM more compared to the brute-force version).
//
// # Note
// I observed a max. recursion depth of 363 (incl. memoization). This shouldn't be a problem for most computers.
//
// # Alternative
// There are surprisingly few "thresholds", where the Collatz chain length is longer than anything we have seen before:
// >       1,      2,      3,      6,      7,      9,     18,     25,     27,     54,
// >      73,     97,    129,    171,    231,    313,    327,    649,    703,    871,
// >    1161,   2223,   2463,   2919,   3711,   6171,  10971,  13255,  17647,  23529,
// >   26623,  34239,  35655,  52527,  77031, 106239, 142587, 156159, 216367, 230631,
// >  410011, 511935, 626331, __837799__,1117065,1501353,1723519,2298025,3064033,3542887,
// > 3732423
//
// This sequence is all we need to solve the original problem (well, this list contains all numbers up to 5000000 but we only need up to 1000000).
// You can easily see that 837799 is the largest number less than one million.
//
// # Hackerrank
// I compute the Collatz sequences "on-demand":
// if the input value ''x'' exceeds ''maxTested'' then all still unknown Collatz sequences up to ''x'' will be analyzed.
// Whenever a sequence is at least as long as the longest sequence we observed so far, then I have to update ''longest''.
//
// This data structure ''longest'' is supposed to hold the values shown above.
// However, the modified Hackerrank problem asks for the __largest__ number with a given Collatz chain length
// but those numbers refers to the __smallest__.
//
// Therefore I have to insert all numbers where
// ''length >= longest.rbegin()->second'' instead of
// ''length >  longest.rbegin()->second''
//
// In the end, my ''std::map'' contains a few more values:
// >       1,      2,      3,      6,      7,      9,     18,     19,     25,     27,
// >      54,     55,     73,     97,    129,    171,    231,    235,    313,    327,
// >     649,    654,    655,    667,    703,    871,   1161,   2223,   2322,   2323,
// >    2463,   2919,   3711,   6171,  10971,  13255,  17647,  17673,  23529,  26623,
// >   34239,  35497,  35655,  52527,  77031, 106239, 142587, 156159, 216367, 230631,
// >  410011, 511935, 626331, 837799,1117065,1126015,1501353,1564063,1723519,2298025,
// > 3064033,3542887,3732423

#include <iostream>
#include <vector>
#include <map>

// memoize all paths length for n up to 5000000
const size_t MaxN = 5000000 + 2;
// we could change MaxN at will:
// it just affects performance, not the result

// identify chain lengths we haven't computed so far
const int Unknown = -1;
// store chain lengths
std::vector<short> cache(MaxN, Unknown);

// recursively count steps of Collatz sequence
unsigned short steps(unsigned long long x)
{
  // finished recursion ?
  if (x == 1)
    return 1;

  // try to use cached result
  if (x < cache.size() && cache[x] != Unknown)
    return cache[x];

  // next step
  long long next;
  if (x % 2 == 0)
    next = x / 2;
  else
    next = 3 * x + 1;

  // deeper recursion
  auto result = 1 + steps(next);
  if (x < cache.size())
    cache[x] = result;

  return result;
}

int main()
{
  // [smallest number] => [chain length]
  std::map<unsigned int, unsigned int> longest;
  // highest number analyzed so far
  unsigned int maxTested = 1;
  longest[maxTested] = 1; // obvious case

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int x;
    std::cin >> x;

    // compute remaining chain lengths
    for (; maxTested <= x; maxTested++)
    {
      // get chain length
      auto length = steps(maxTested);
      // at least as long as anything we have seen before ?
      if (length >= longest.rbegin()->second)
        longest[maxTested] = length;
    }

    // find next longest chain for numbers bigger than x
    auto best = longest.upper_bound(x);
    // and go one step back
    best--;

    std::cout << best->first << std::endl;
  }
  return 0;
}
