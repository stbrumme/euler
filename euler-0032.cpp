// ////////////////////////////////////////////////////////
// # Title
// Pandigital products
//
// # URL
// https://projecteuler.net/problem=32
// http://euler.stephan-brumme.com/32/
//
// # Problem
// We shall say that an `n`-digit number is pandigital if it makes use of all the digits `1` to `n` exactly once;
// for example, the 5-digit number, `15234`, is 1 through 5 pandigital.
//
// The product `7254` is unusual, as the identity, `39 * 186 = 7254`, containing multiplicand, multiplier, and product is 1 through 9 pandigital.
//
// Find the sum of all products whose multiplicand/multiplier/product identity can be written as a 1 through 9 pandigital.
// __HINT:__ Some products can be obtained in more than one way so be sure to only include it once in your sum.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// We have to solve an equation `a * b = c`.
// There are `9! = 362880` permutations of ''{ 1,2,3,4,5,6,7,8,9 }'' and for each of those permutation I try all possible combinations of `a`, `b` and `c`.
//
// All digits (0..9) are stored in a plain ''std::vector'' and the STL's function ''std::next_permutation'' generates all permutations.
// Then two nested loops split the sequence into three parts (`a` has length ''lenA'', `b` has length ''lenB'' and `c` has length ''lenC'').
// If `a * b = c` then my program stores the product `c` in ''std::set'' named ''valid''. Duplicates are automatically avoided by the ''std::set''.
//
// Finally all elements of that ''std::set'' are added.
//
// # Note
// There are many opportunities for optimizations but the code is already extremely fast.
//
// # Hackerrank
// The digits are 1 to n instead of 1 to 9.

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

int main()
{
  // read highest digit
  unsigned int maxDigit;
  std::cin >> maxDigit;

  // all digits from 1..9
  std::vector<unsigned int> digits = { 1,2,3,4,5,6,7,8,9 };
  // remove higher numbers so there is only 1..n left
  digits.resize(maxDigit);

  // all pandigital products
  std::set<unsigned int> valid;

  // create all permutations
  do
  {
    // let's say a * b = c
    // each variable contains at least one digit
    // the sum of their digits is limited by n (which should be 9)
    // try all combinations of lengths with the current permutation of digits
    for (unsigned int lenA = 1; lenA < maxDigit; lenA++)
      for (unsigned int lenB = 1; lenB < maxDigit - lenA; lenB++)
      {
        unsigned int lenC = maxDigit - lenA - lenB;

        // a*b=c => c>=a && c>=b => c has at least as many digits as a or b
        if (lenC < lenA || lenC < lenB)
          break;

        // pos contains the currently used position in "digits"
        unsigned int pos = 0;

        // build "a" out of the first digits
        unsigned int a = 0;
        for (unsigned int i = 1; i <= lenA; i++)
        {
          a *= 10;
          a += digits[pos++];
        }

        // next digits represent "b"
        unsigned int b = 0;
        for (unsigned int i = 1; i <= lenB; i++)
        {
          b *= 10;
          b += digits[pos++];
        }

        // and the same for "c"
        unsigned int c = 0;
        for (unsigned int i = 1; i <= lenC; i++)
        {
          c *= 10;
          c += digits[pos++];
        }

        // is a*b = c ?
        if (a*b == c)
          valid.insert(c);
      }
  } while (std::next_permutation(digits.begin(), digits.end()));

  // find sum
  unsigned int sum = 0;
  for (auto x : valid)
    sum += x;
  std::cout << sum << std::endl;

  return 0;
}
