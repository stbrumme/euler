// ////////////////////////////////////////////////////////
// # Title
// Ordered radicals
//
// # URL
// https://projecteuler.net/problem=124
// http://euler.stephan-brumme.com/124/
//
// # Problem
// The radical of `n`, `rad(n)`, is the product of the distinct prime factors of `n`.
// For example, `504 = 23 * 32 * 7`, so `rad(504) = 2 * 3 * 7 = 42`.
//
// If we calculate `rad(n)` for `1 <= n <= 10`, then sort them on `rad(n)`, and sorting on `n` if the radical values are equal, we get:
//
// ||  3 ||    3   ||  4 ||  3 || 3      ||  3 ||
// ||   Unsorted   ||||  ||    Sorted    ||||  ||!
// ||  n || rad(n) ||    ||  n || rad(n) ||  k ||!
// ||  1 ||    1   ||    ||  1 ||    1   ||  1 ||
// ||  2 ||    2   ||    ||  2 ||    2   ||  2 ||
// ||  3 ||    3   ||    ||  4 ||    2   ||  3 ||
// ||  4 ||    2   ||    ||  8 ||    2   ||  4 ||
// ||  5 ||    5   ||    ||  3 ||    3   ||  5 ||
// ||  6 ||    6   ||    ||  9 ||    3   ||  6 ||
// ||  7 ||    7   ||    ||  5 ||    5   ||  7 ||
// ||  8 ||    2   ||    ||  6 ||    6   ||  8 ||
// ||  9 ||    3   ||    ||  7 ||    7   ||  9 ||
// || 10 ||   10   ||    || 10 ||   10   || 10 ||
//
// Let `E(k)` be the kth element in the sorted n column; for example, `E(4) = 8` and `E(6) = 9`.
//
// If `rad(n)` is sorted for `1 <= n <= 100000`, find `E(10000)`.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// Similar to a prime sieve I find all multiples of each prime.
// There is one struct ''Radical'' for every number ''n'', its ''product'' starts with ''1'' and is multiplied by a prime factors of ''n''.
// A simple comparison ''operator<()'' will be used to sort all radicals.
//
// A full sort isn't needed - there is the nice ''nth_element'' algorithm in C++'s STL which ensures that a single element is at the correct position -
// without a strict guarantee that other elements are perfectly sorted, too. This partial sort is often much faster than a full sort.
//
// # Hackerrank
// I don't properly re-use radicals for consecutive test cases and can't handle huge search spaces. I couldn't care less ...

#include <iostream>
#include <vector>
#include <algorithm>

// store a single radical
struct Radical
{
  // current number
  unsigned int n;
  // product of all prime factors
  unsigned int product;

  // compare two object, prefer lower product (and lower n if products are equal)
  bool operator<(const Radical& other) const
  {
    if (product != other.product)
      return product < other.product;
    return n < other.n;
  }
};

// all relevant redicals
std::vector<Radical> rads;

// return a certain radical (1-based index)
unsigned int getNth(unsigned int index)
{
  index--; // 1-based instead of 0-based
  // partial sort
  std::nth_element(rads.begin(), rads.begin() + index, rads.end());
  return rads[index].n;
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int limit = 100000;
    std::cin >> limit;

    // initialize all radicals
    rads.clear();
    rads.reserve(limit + 1);
    for (unsigned int i = 0; i <= limit; i++)
    {
      Radical current;
      current.n = i;
      current.product = 1;
      rads.push_back(current);
    }

    // some big value to push zero to the back
    rads[0].product = 999999999;

    // compute radicals using a "sieve"
    for (auto current : rads)
    {
      // process only primes
      if (current.product != 1)
        continue;

      // adjust all of their multiples
      for (unsigned int j = current.n; j <= limit; j += current.n)
        rads[j].product *= current.n;
    }

    // get n-th element
    unsigned int pos = 10000;
    std::cin >> pos;
    std::cout << getNth(pos) << std::endl;
  }
  return 0;
}
