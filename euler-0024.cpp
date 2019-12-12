// ////////////////////////////////////////////////////////
// # Title
// Lexicographic permutations
//
// # URL
// https://projecteuler.net/problem=24
// http://euler.stephan-brumme.com/24/
//
// # Problem
// A permutation is an ordered arrangement of objects. For example, 3124 is one possible permutation of the digits 1, 2, 3 and 4.
// If all of the permutations are listed numerically or alphabetically, we call it lexicographic order.
//
// The lexicographic permutations of 0, 1 and 2 are:
// 012   021   102   120   201   210
//
// What is the millionth lexicographic permutation of the digits 0, 1, 2, 3, 4, 5, 6, 7, 8 and 9?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The original problem can be solved with STL's handy ''std::next_permutation''.
// Even though we call it 999999 times it is still very fast (< 10 ms).
//
// __Note:__ the first permutation has index 0, hence 999999 instead of 1000000 iterations.
//
// # Alternative
// Hackerrank's modified problem has a much larger "search space" and causes timeout.
// Therefore I implemented an alternative algorithm based on the "Factorial number system" (see https://en.wikipedia.org/wiki/Factorial_number_system ):
//
// 499999 in our decimal system is:
// `4 * 10^4 + 9 * 10^4 + 9 * 10^3 + 9 * 10^2 + 9 * 10^1 + 9 * 10^0`
//
// The factorial number system replaces `10^x` by `x!`:
// `1 * 9! + 3 * 8! + 3 * 7! + 1 * 6! + 2 * 5! + 3 * 4! + 1 * 3! + 0 * 2! + 1 * 1! + 0 * 0!`
// `= 1 * 362880 + 3 * 40320 + 3 * 5040 + 1 * 720 + 2 * 120 + 3 * 24 + 1 * 6 + 0 * 2 + 1 * 1 + 0 * 1`
// `= 499999`
//
// The coefficients 1, 3, 3, 1, 2, 3, 1, 0, 1, 0 define which indices of our original, unpermutated string we have to choose.
// But there is a twist: whenever we select an element, we have to remove it from the original. And everything's 0-based.
// That means:
// ''0123456789'' ==> choose element 1: ''1'' ==> ''1''
// ''023456789 '' ==> choose element 3: ''4'' ==> ''14''
// ''02356789  '' ==> choose element 3: ''5'' ==> ''145''
// ''0236789   '' ==> choose element 1: ''2'' ==> ''1452''
// ''036789    '' ==> choose element 2: ''6'' ==> ''14526''
// ''03789     '' ==> choose element 3: ''8'' ==> ''145268''
// ''0379      '' ==> choose element 1: ''3'' ==> ''1452683''
// ''079       '' ==> choose element 0: ''0'' ==> ''14526830''
// ''79        '' ==> choose element 1: ''9'' ==> ''145268309''
// ''7         '' ==> choose element 0: ''7'' ==> ''1452683097''
//
// # Note
// The "live test" is based on the Hackerrank problem.
//
// # Hackerrank
// The string ''abcdefghijklm'' is used instead of ''0123456789''.
// In the end, we have 13! instead of 10! potential permutations (1715x more permutations).

#include <iostream>
#include <string>
#include <algorithm>

int main()
{
//#define ORIGINAL
#ifdef ORIGINAL
  unsigned int numPermutation = 1000000;
  std::string current = "0123456789";
  while (--numPermutation)
    std::next_permutation(current.begin(), current.end());
  std::cout << current << std::endl;

#else

  const std::string abc = "abcdefghijklm";
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // to find the permutation we treat the input number as something written
    // in a "factorial" system:
    // x = pos0 * 12! + pos1 * 11! + pos2 * 10! + ... + pos12 * 1!
    // (we have 13 letters, therefore the position of the first letter is in 12! radix)

    // precomputed 0! .. 12!
    const unsigned long long factorials[13+1] =
    { 1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,479001600,6227020800 };

    // 13! which exceed 32 bits
    unsigned long long x;
    std::cin >> x;

    // reduce to a single cycle (repeats after 13! iterations)
    x %= factorials[abc.size()];

    // that factorial system is zero-based ...
    x--;

    // strip off single letters (until empty)
    auto remain = abc;
    // our wanted permutation
    std::string result;
    while (!remain.empty())
    {
      // get next digit in that strange number system :-)
      auto currentFactorial = factorials[remain.size() - 1];
      auto pos = x / currentFactorial;

      // store the associated letter
      result += remain[pos];
      // and remove it from the still unprocessed data
      remain.erase(pos, 1);

      // eliminate the processed digit
      x %= currentFactorial;
    }

    std::cout << result << std::endl;
  }
#endif
  return 0;
}
