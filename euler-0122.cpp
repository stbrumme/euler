// ////////////////////////////////////////////////////////
// # Title
// Efficient exponentiation
//
// # URL
// https://projecteuler.net/problem=122
// http://euler.stephan-brumme.com/122/
//
// # Problem
// The most naive way of computing `n^15` requires fourteen multiplications:
// `n * n * ... * n = n^15`
//
// But using a "binary" method you can compute it in six multiplications:
//
// `n * n = n^2`
// `n^2 * n^2 = n^4`
// `n^4 * n^4 = n^8`
// `n^8 * n^4 = n^12`
// `n^12 * n^2 = n^14`
// `n^14 * n = n^15`
//
// However it is yet possible to compute it in only five multiplications:
//
// `n * n = n^2`
// `n^2 * n = n^3`
// `n^3 * n^3 = n^6`
// `n^6 * n^6 = n^12`
// `n^12 * n^3 = n^15`
//
// We shall define `m(k)` to be the minimum number of multiplications to compute `n^k`; for example `m(15) = 5`.
//
// For `1 <= k <= 200`, find `sum{m(k)}`.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// It took me quite some time to figure out a fast solution.
// I realized pretty soon that I should generate a sequence which contains the exponents where `a_i = a_k + a_l` and `i > k >= l`.
// A sequence for `n^15` is `{ 1, 2, 3, 6, 12, 15 }`. Using pen and paper I also found `{ 1, 2, 4, 5, 10, 15 }`.
// Therefore there is not one unique optimal solution for each exponent but there can be multiple.
//
// First I tried an iterative depth-first search, where I append any combination of `a_k + a_l` to a list which starts with `{ 1 }`.
// The number of combinations was huge - too much for my little computer ...
// Then I discovered that this problem is called "Addition chain" and there is substantial information available: https://en.wikipedia.org/wiki/Addition_chain
// A list of chain lengths can be downloaded, too: http://wwwhomes.uni-bielefeld.de/achim/addition_chain.html
//
// Especially the Brauer chain caught my eye: for `n <= 2500` you find the correct solution by restricting `k = i - 1`.
// In plain English: add every exponent to the biggest one only.
//
// ''search'' is a recursive depth-first search, limited by ''maxDepth'':
// - it aborts immediately if search depth was exhausted, e.g. the current chain is longer than allowed
// - it computes the sum of any element and the last element and returns ''true'' if it matches the ''exponent''
// - else it appends the sum and has to go deeper
// The code runs about twice as fast when combining "high" numbers first: instead of `a_0 + a_{last}, a_1 + a_{last}, ... a_{last} + a_{last}` I start the other way around.
//
// ''findChain'' slowly increases the search depth ''search'' returns a solution.
//
// # Hackerrank
// Each solution has to be ''print''ed in a human-readable format.
// Multiple test cases want you to compute the same formulas, therefore a small cache was added.

#include <iostream>
#include <vector>
#include <map>

//#define ORIGINAL

// a single addition chain
typedef std::vector<unsigned int> Chain;

// iterative depth-first search of Brauer sequence
bool search(Chain& chain, unsigned int exponent, unsigned int maxDepth)
{
  // too deep ?
  if (chain.size() > maxDepth)
    return false;

  auto last = chain.back();
  for (size_t i = 0; i < chain.size(); i++)
  {
    //auto sum = chain[i] + last;
    auto sum = chain[chain.size() - 1 - i] + last; // try high exponents first => about twice as fast
    if (sum == exponent)
      return true;

    chain.push_back(sum);
    if (search(chain, exponent, maxDepth))
      return true;

    chain.pop_back();
  }

  return false;
}

// increase depth until a solution is found
Chain findChain(unsigned int exponent)
{
  // cached ? (needed for Hackerrank only)
  static std::map<unsigned int, Chain> cache;
  auto lookup = cache.find(exponent);
  if (lookup != cache.end())
    return lookup->second;

  // start iterative search
  Chain chain;
  unsigned int depth = 1;
  while (true)
  {
    // reset chain
    chain = { 1 };
    // a start search
    if (search(chain, exponent, depth))
      break;

    // failed, allow to go one step deeper
    depth++;
  }

  cache[exponent] = chain;
  return chain;
}

// print a single chain in Hackerrank format
void print(const Chain& chain)
{
  // number of multiplications
  std::cout << (chain.size() - 1) << std::endl;
  // print each multiplication
  for (size_t i = 1; i < chain.size(); i++)
  {
    // involved exponents
    auto sum  = chain[i];
    auto add1 = chain[i - 1];
    auto add2 = sum - add1;

    std::cout << "n";
    if (add1 > 1)
      std::cout << "^" << add1;
    std::cout << " * n";
    if (add2 > 1)
      std::cout << "^" << add2;
    std::cout << " = n^" << sum << std::endl;
  }
}

int main()
{
#ifdef ORIGINAL

  unsigned int sum = 0;
  // find all chains 2..200
  for (unsigned int exponent = 2; exponent <= 200; exponent++)
  {
    auto chain = findChain(exponent);
    // sum of all chains' lengths
    sum += chain.size();
  }
  std::cout << sum << std::endl;

#else

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int exponent;
    std::cin >> exponent;

    // compute one chain (there might be different chains of the same length)
    auto chain = findChain(exponent);
    // append the exponent, which is not part of the chain yet
    chain.push_back(exponent);
    // and display
    print(chain);
  }

#endif

  return 0;
}
