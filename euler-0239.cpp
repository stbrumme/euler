// ////////////////////////////////////////////////////////
// # Title
// Twenty-two Foolish Primes
//
// # URL
// https://projecteuler.net/problem=239
// http://euler.stephan-brumme.com/239/
//
// # Problem
// A set of disks numbered 1 through 100 are placed in a line in random order.
//
// What is the probability that we have a partial derangement such that exactly 22 prime number discs are found away from their natural positions?
// (Any number of non-prime disks may also be found in or out of their natural positions.)
//
// Give your answer rounded to 12 places behind the decimal point in the form 0.abcdefghijkl.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// Let's simplify this problem:
// - there are 25 prime numbers below 100
// - exactly 25 - 22 = 3 must remain at their position
// - actually it doesn't matter whether those fixed numbers are prime numbers or not !
// - therefore I just look at the first 25 numbers (1 .. 25) and require that three of them are fixed
//
// The Wikipedia article on Derangement contains all the needed formulas: https://en.wikipedia.org/wiki/Derangement
// The "subfactorial" (I haven't heard that name before !) is the core concept behind my ''derangements()'' function.
//
// ''derangements()'' tells me the number of ways such that the first three numbers are fixed.
// There are 2300 ways of choosing any three primes out of the 25 available (''choose(25,3)'').
// Now I a total count of deranged sets - dividing it by the number of permutations (it's `100!`) gives the probability.
//
// # Note
// Unlike most of my solutions, this time ''choose()'' and ''factorial()'' return ''double'' because their results are really big and their last digits don't matter.
// There is an opportunity to pre-compute the factorials but my program terminates after less than 0.01 seconds even without this optimization.
//
// I was a bit confused whether my result has to be the "raw" result or a "percentage" (thus multiplied by 100).
// Strangely enough, my first attempt was correct - that's usually never happens ...
//
// # Alternative
// A different approach can be found on http://www.numericana.com/answer/counting.htm although they don't go into detail
// (except mentioning inclusion-exclusion principle, see https://en.wikipedia.org/wiki/Inclusion%E2%80%93exclusion_principle )

#include <iostream>
#include <iomanip>

// ---------- based on similar code in my toolbox ----------

// factorial
// result is not accurate but supports large ranges
double factorial(unsigned int n)
{
  double result = 1;
  while (n > 1)
    result *= n--;
  return result;
}

// number of ways to choose n elements from k available
double choose(unsigned int n, unsigned int k)
{
  // n! / (n-k)!k!
  return factorial(n) / (factorial(n - k) * factorial(k));
}

// ---------- problem-specific code ----------

// count derangement
// note: need double as return type because results will be HUGE
double derangements(unsigned int move, unsigned int dontCare)
{
  // don't need to move a prime away from its original position ?
  if (move < 1)
    return factorial(dontCare); // permutation of all remaining numbers

  // recursion
  move--;
  auto result = dontCare * derangements(move,     dontCare);
  if (move > 0)
    result   += move     * derangements(move - 1, dontCare + 1);

  return result;
}

int main()
{
  unsigned int disks  = 100;
  unsigned int primes =  25;
  unsigned int moved  =  22;
  std::cin >> moved;

  // detect invalid input: for live test only
  if (moved > primes)
    return 1;

  unsigned int unchanged = primes - moved;

  // count ways
  double result = derangements(moved, disks - primes);

  // => 2300 ways to choose 3 primes
  result *= choose(primes, unchanged);
  // divide by total number of permutations
  result /= factorial(disks);

  // display result
  std::cout << std::fixed << std::setprecision(12) << result << std::endl;
  return 0;
}
