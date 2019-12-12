// ////////////////////////////////////////////////////////
// # Title
// Non-bouncy numbers
//
// # URL
// https://projecteuler.net/problem=113
// http://euler.stephan-brumme.com/113/
//
// # Problem
// Working from left-to-right if no digit is exceeded by the digit to its left it is called an increasing number; for example, 134468.
// Similarly if no digit is exceeded by the digit to its right it is called a decreasing number; for example, 66420.
//
// We shall call a positive integer that is neither increasing nor decreasing a "bouncy" number; for example, 155349.
//
// As `n` increases, the proportion of bouncy numbers below n increases such that there are only 12951 numbers below one-million
// that are not bouncy and only 277032 non-bouncy numbers below `10^10`.
//
// How many numbers below a googol (`10^100`) are not bouncy?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// My solution is based on a dynamic programming approach:
// - solve the problem for one digit
// - solve the problem for n+1 digits by using information from n digits
//
// All 9 single digits are not bouncy. Therefore I initialize my array ''increase'' and ''decrease'' with 1s.
// Each of their entries represents how many numbers starting with a digit are not bouncy.
// In ''increase[x][y]'' you find the count of increasing numbers with ''x'' digits, where the front-most digit is ''y''.
//
// In each iteration, ''increase'' and ''decrease'' are updated:
// ''increase[x][y]'' is the sum of all ''increase[x-1][less than or equal to y]'' (and the other way around for ''decrease'', too).
// All not bouncy numbers are either increasing or decreasing. I have to deduct all increasing numbers where the first digit is zero.
// On top of that, numbers where all digits are identical are both increasing und decreasing and counted twice, therefore I have to subtract 10.
//
// # Hackerrank
// More than 100 digits are no problem. The result will be a rather large number and has to be printed `mod 10^9 + 7`.

#include <iostream>
#include <vector>

// print result modulo some value
#define ORIGINAL
#ifdef ORIGINAL
const unsigned long long Modulo = 1000000000000000000ULL; // high enough to keep the result unchanged
#else
const unsigned long long Modulo = 1000000007ULL;
#endif

int main()
{
  // Googol = 100, but Hackerrank wants more ...
  const unsigned int numDigits = 100000;
  std::vector<unsigned long long> solutions(numDigits + 1, 0);

  // count how many numbers are increasing and/or decreasing
  typedef unsigned long long DigitCounter[10]; // some older GCC complain about using this in a vector
  std::vector<DigitCounter> increase(numDigits);
  std::vector<DigitCounter> decrease(numDigits);

  // all one-digit numbers are non-bouncy
  unsigned long long sum = 9;
  for (auto& x : increase[0])
    x = 1;
  for (auto& x : decrease[0])
    x = 1;

  // process digits, beginning from the right side
  for (unsigned int i = 1; i < numDigits; i++)
  {
    // digits 0..9
    for (unsigned int current = 0; current <= 9; current++)
    {
      // add count of all numbers where the next digit is equal or lower
      decrease[i][current] = 0;
      for (unsigned int smaller = 0; smaller <= current; smaller++)
        decrease[i][current] = (decrease[i][current] + decrease[i - 1][smaller]) % Modulo;

      // add count of all numbers where the next digit is equal or higher
      increase[i][current] = 0;
      for (unsigned int bigger = current; bigger <= 9; bigger++)
        increase[i][current] = (increase[i][current] + increase[i - 1][bigger]) % Modulo;
    }

    // compute total sum of increasing and decreasing numbers
    for (auto x : increase[i])
      sum += x;
    for (auto x : decrease[i])
      sum += x;

    // but numbers must not start with zero !
    sum -= increase[i][0];

    // numbers with identical digits were counted twice
    // because they are both increasing and decreasing (e.g. 55555555)
    sum -= 10;

    // Hackerrank only
    sum %= Modulo;
    solutions[i] = sum;
  }

  // lookup results
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int digits;
    std::cin >> digits;
    std::cout << solutions[digits - 1] << std::endl; // 0-based array but 1-based input
  }

  return 0;
}
