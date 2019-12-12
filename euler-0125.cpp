// ////////////////////////////////////////////////////////
// # Title
// Palindromic sums
//
// # URL
// https://projecteuler.net/problem=125
// http://euler.stephan-brumme.com/125/
//
// # Problem
// The palindromic number `595` is interesting because it can be written as the sum of consecutive squares: `6^2 + 7^2 + 8^2 + 9^2 + 10^2 + 11^2 + 12^2`.
//
// There are exactly eleven palindromes below one-thousand that can be written as consecutive square sums, and the sum of these palindromes is `4164`.
// Note that `1 = 0^2 + 1^2` has not been included as this problem is concerned with the squares of positive integers.
//
// Find the sum of all the numbers less than 108 that are both palindromic and can be written as the sum of consecutive squares.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// My function ''isPalindrome'' determines whether it's parameter is a palindrome or not:
// - iteratively remove the lowest digit and append it to ''reverse''
// - if at the end ''reverse == x'' then we have a palindrome
// A small optimization is to check the right-most digit for zero because then it can't be a palindrome.
//
// Two nested loops iterate over all possible sequences.
// My first solution wasn't correct because I didn't notice that sums may appear multipe times (there are two "collisions" for the original problem).
// All palindromes are temporarily stored in ''solutions'' and then I remove all duplicates (see http://oeis.org/A267600 ).
//
// # Alternative
// There is a closed formula for a sequence `sum_{a=i..j}{a^2}` but I'm not sure whether it would be faster because my incremental computation of ''current'' is extremely efficient.
//
// # Hackerrank
// The upper limit can be defined as well as the distance between consecutive "numbers" (what I call ''stride''):
// instead of `a^2 + (a+1)^2 + (a+2)^2 + ...` the terms are `a^2 + (a+stride)^2 + (a+2 * stride)^2 + ...`

#include <vector>
#include <iostream>
#include <algorithm>

// return true if x is a palindrome
bool isPalindrome(unsigned int x)
{
  auto reduced = x / 10;
  auto reverse = x % 10;
  // fast exit: a trailing zero can't create a palindrome
  if (reverse == 0)
    return false;

  while (reduced > 0)
  {
    // chop off the lowest digit and append it to "reverse"
    reverse *= 10;
    reverse += reduced % 10;
    reduced /= 10;
  }

  // palindrome ? both must be equal
  return reverse == x;
}


int main()
{
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int limit  = 100000000;
    unsigned int stride = 1; // distance between consecutive square numbers

    std::cin >> limit >> stride;

    std::vector<unsigned int> solutions;
    for (unsigned long long first = 1; 2*first*first < limit; first++)
    {
      auto next = first + stride;
      // sum of a^2 + b^2 + ...
      auto current = first * first + next * next;
      // still within the limit ?
      while (current < limit)
      {
        // check
        if (isPalindrome(current))
          solutions.push_back(current);

        // add one element to the sequence
        next    += stride;
        current += next * next;
      }
    }

    // sort ...
    std::sort(solutions.begin(), solutions.end());
    // .. and remove duplicates
    auto garbage = std::unique(solutions.begin(), solutions.end());
    solutions.erase(garbage, solutions.end());

    // count all solutions
    unsigned long long sum = 0;
    for (auto x : solutions)
      sum += x;

    std::cout << sum << std::endl;
  }

  return 0;
}
