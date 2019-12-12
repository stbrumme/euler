// ////////////////////////////////////////////////////////
// # Title
// Combinatoric selections
//
// # URL
// https://projecteuler.net/problem=53
// http://euler.stephan-brumme.com/53/
//
// # Problem
// There are exactly ten ways of selecting three from five, 12345:
//
// 123, 124, 125, 134, 135, 145, 234, 235, 245, and 345
//
// In combinatorics, we use the notation, `^5C_3 = 10`.
//
// In general,
// `^nC_r = dfrac{n!}{r! * (n-r)!}` where `r <= n`, `n! = n * (n-1) * ... * 3 * 2 * 1`, and `0! = 1`.
//
// It is not until `n = 23`, that a value exceeds one-million: `^{23}C_10 = 1144066`.
//
// How many, not necessarily distinct, values of `^nC_r`, for `1 <= n <= 100`, are greater than one-million?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The formulas based on factorials (those provided in the problem statement) allow a direct computation of `^nC_r`.
// However, you might get very big numbers in the numerator and/or denominator - they easily exceed the range of a 32 or 64 bit integer.
//
// There is another formula - the recursive definition:
// - `^nC_0 = {^nC_n} = 1` and
// - `^nC_k = {^{n-1}C_{k-1}} + {^{n-1}C_k}`
//
// As soon as any `^nC_k` exceeds ''maxNumber'', I set it to ''maxNumber + 1''. This value still fits into an 64 bit integer (even ''2*(maxNumber+1)'' is no problem).
// At the same time, ''bigNumbers'' is incremented.
//
// I am allowed to replace `^nC_k` by ''maxNumber + 1'' because the true value of `^nC_k` doesn't really matter - all we want to know is whether `^nC_k > maxNumber` - or not.
//
// When all values are processed, ''bigNumbers'' is displayed.
//
// # Note
// The program could use less memory: instead of storing all values of ''combinations[n][k]'' it is sufficient to keep only
// ''combinations[n-1][]'' and ''combinations[n][]'', thus reducing the memory requirements from `maxN^2` to `2*maxN`.

#include <vector>
#include <iostream>

int main()
{
  // maximum index n and/or k
  unsigned int       maxN      = 100;
  // what is considered "big" ?
  unsigned long long maxNumber = 1000000;
  std::cin >> maxN >> maxNumber;

  // this will be the displayed result
  unsigned int bigNumbers = 0;

  // setup a 2D array to hold all values
  std::vector<std::vector<unsigned long long>> combinations(maxN + 1);

  // C(n,0) = C(n,n) = 1
  for (unsigned int n = 0; n <= maxN; n++)
  {
    combinations[n].resize(n + 1, 0);
    combinations[n][0] = combinations[n][n] = 1;
  }

  // recursive definition:
  // C(n,k) = C(n-1, k-1) + C(n-1, k)
  for (unsigned int n = 1; n <= maxN; n++)
    for (unsigned int k = 1; k < n; k++)
    {
      auto sum = combinations[n - 1][k - 1] + combinations[n - 1][k];
      // clamp numbers to avoid exceeding 64 bits
      if (sum > maxNumber)
      {
        sum = maxNumber + 1;
        // we found one more big number
        bigNumbers++;
      }

      // store result
      combinations[n][k] = sum;
    }

  std::cout << bigNumbers << std::endl;
  return 0;
}
