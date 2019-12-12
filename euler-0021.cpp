// ////////////////////////////////////////////////////////
// # Title
// Amicable numbers
//
// # URL
// https://projecteuler.net/problem=21
// http://euler.stephan-brumme.com/21/
//
// # Problem
// Let `d(n)` be defined as the sum of proper divisors of `n` (numbers less than `n` which divide evenly into `n`).
// If `d(a) = b` and `d(b) = a`, where `a!=b`, then `a` and `b` are an amicable pair
// and each of `a` and `b` are called amicable numbers.
//
// For example, the proper divisors of 220 are 1, 2, 4, 5, 10, 11, 20, 22, 44, 55 and 110; therefore `d(220) = 284`.
// The proper divisors of 284 are 1, 2, 4, 71 and 142; so `d(284) = 220`.
//
// Evaluate the sum of all the amicable numbers under 10000.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My function ''getSum'' returns the sum of all proper divisors of ''x''.
// The brute-force approach works but turns out to be too slow: a ''for''-loop from 2 to ''x-1''.
//
// But we can easily reduce the number of iterations:
// For each proper divisor `i` of `x` there is another divisor `j=x/i`, except when `i^2=x`.
// More interesting, if we assume `i<j` then `i<=sqrt{x}`.
//
// A simple loop running from 2 to `sqrt{x}` check whether `x mod i == 0` (proper divisor)
// and then adds `i` as well as `j` (if `i!=j`) to the result.
//
// A precomputation step in ''main'' finds the sums of all divisors of all numbers ''i'' below 100000 and calls it ''sibling = getSum(i)''.
// If ''getSum(sibling) = i'' then ''i,sibling'' is an amicable pair.
//
// Those two numbers are stored in an ''std::set'' named ''amicables''.
// For each test case, the sum of all relevant numbers is printed.

#include <iostream>
#include <set>

// generate sum of all divisor's of x (where x > 1)
unsigned int getSum(unsigned int x)
{
  // find all factors:
  // look only for the "smaller" divisors <= sqrt(x)
  // and they have a "bigger" brother x / divisor

  // 1 is always a divisor, but not the number x itself
  unsigned int divisorSum = 1;
  // therefore start at 2
  for (unsigned int divisor = 2; divisor * divisor <= x; divisor++)
    if (x % divisor == 0)
    {
      divisorSum += divisor;

      // add the "bigger brother"
      auto otherDivisor = x / divisor;
      // except square numbers
      if (otherDivisor != divisor)
        divisorSum += otherDivisor;
    }

  return divisorSum;
}

int main()
{
  // contain all numbers which are part of an amicable pair
  std::set<unsigned int> amicables;

  // precomputation step:
  // find all amicable numbers <= 100000
  const unsigned int MaxAmicable = 100000;
  for (unsigned int i = 2; i <= MaxAmicable; i++)
  {
    auto sibling = getSum(i);

    // found a pair ?
    if (i == getSum(sibling) && i != sibling)
    {
      amicables.insert(i);
      amicables.insert(sibling);
    }
  }

  // and now start processing input
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int x;
    std::cin >> x;

    // just look up all suitables numbers
    unsigned int sum = 0;
    for (auto i : amicables)
    {
      // discard those that are too big
      if (i > x)
        break;
      // note: an set::set is sorted ascendingly by default

      // yes, accept that amicable number
      sum += i;
    }

    std::cout << sum << std::endl;
  }
  return 0;
}
