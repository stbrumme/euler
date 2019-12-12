// ////////////////////////////////////////////////////////
// # Title
// Largest palindrome product
//
// # URL
// https://projecteuler.net/problem=4
// http://euler.stephan-brumme.com/4/
//
// # Problem
// A palindromic number reads the same both ways.
// The largest palindrome made from the product of two 2-digit numbers is `9009 = 91 * 99`.
// Find the largest palindrome made from the product of two 3-digit numbers.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The upper three digits of such a six-digit palindrome are a "mirrored" version of the lower three digits.
// I wrote a function ''makePalindrome'' which takes a three-digit number and returns its six-digit palindrome.
//
// There are only 899 six-digit palindromes because the upper three digits must be 100 ... 999
// (no leading zeros allowed because then it wouldn't be a six-digit number anymore).
//
// Beginning with 999, I loop "downwards" through all possible combinations trying to find a three-digit divisor.
// A simple speedup is achieved by observing that at least one divisor `i` must be `i>=100` and `i^2<=palindrome`.
//
// # Hackerrank
// Hackerrank's problem asks for a variable maximum upper limit. It's 999999 for the original problem.
//
// # Note
// I create six-digit palindromes by splitting a three-digit numbers into its single digits.
// A possible alternative is to convert the three-digit number into a string and concatenate it with its reversed version.
// However, often those string operations tend to be quite slow.

#include <iostream>

// convert 3 digits to a 6 digit palindrome by mirroring and appending these 3 digits
// e.g. 234 becomes 234432
unsigned int makePalindrome(unsigned int x)
{
  unsigned int result = x * 1000;   // abc => abc000
  result +=    x / 100;             // a.. => a..00a
  result +=  ((x / 10) % 10) *  10; // .b. => .b.0b.
  result +=   (x % 10)       * 100; // ..c => ..cc..
  return result;
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // Hackerrank has a variable upper limit (instead of 1000000)
    unsigned int maximum;
    std::cin >> maximum;

    bool found = false;
    // find all palindromes, beginning with the largest
    // walk through all three-digit numbers
    for (auto upper3 = maximum / 1000; upper3 >= 100 && !found; upper3--)
    {
      // "mirror" these three digits to create a six-digit palindrome
      auto palindrome = makePalindrome(upper3);
      // too big ?
      if (palindrome >= maximum)
        continue;

      // split into two factors
      for (unsigned int i = 100; i * i <= palindrome; i++)
        if (palindrome % i == 0) // divisible ?
        {
          // make sure both factors must have three digits
          auto other = palindrome / i;
          if (other < 100 || other > 999)
            continue;

          std::cout << palindrome << std::endl;
          found = true;
          break;
        }
    }
  }
  return 0;
}
