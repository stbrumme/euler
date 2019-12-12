// ////////////////////////////////////////////////////////
// # Title
// Lychrel numbers
//
// # URL
// https://projecteuler.net/problem=55
// http://euler.stephan-brumme.com/55/
//
// # Problem
// If we take 47, reverse and add, 47 + 74 = 121, which is palindromic.
//
// Not all numbers produce palindromes so quickly. For example,
// 349 + 943 = 1292,
// 1292 + 2921 = 4213
// 4213 + 3124 = 7337
// That is, 349 took three iterations to arrive at a palindrome.
//
// Although no one has proved it yet, it is thought that some numbers, like 196, never produce a palindrome.
// A number that never forms a palindrome through the reverse and add process is called a Lychrel number.
// Due to the theoretical nature of these numbers, and for the purpose of this problem, we shall assume that a number is Lychrel until proven otherwise.
// In addition you are given that for every number below ten-thousand, it will either
// (i) become a palindrome in less than fifty iterations, or,
// (ii) no one, with all the computing power that exists, has managed so far to map it to a palindrome.
// In fact, 10677 is the first number to be shown to require over fifty iterations before producing a palindrome: 4668731596684224866951378664 (53 iterations, 28-digits).
//
// Surprisingly, there are palindromic numbers that are themselves Lychrel numbers; the first example is 4994.
//
// How many Lychrel numbers are there below ten-thousand?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Intermediate numbers may become quite huge (as hinted in the problem), therefore I decided to store all numbers in a ''BigNumber'',
// which is a ''std::vector'' where each digit is stored as a single element (lowest digits first), e.g. 3401 = { 1, 0, 4, 3 }.
//
// Then, reversing the digits becomes super-easy:
// ''auto reverse = number;''
// ''std::reverse(reverse.begin(), reverse.end());''
//
// And checking whether we have a palindrome
// ''if (number == reverse)''
//
// Adding the digits is based on the basic algorithm taught in school.

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

//#define ORIGINAL

// a sequence of digits, lowest digits first, e.g. 3401 = { 1, 0, 4, 3 }
typedef std::vector<unsigned int> BigNumber;

// count all numbers converging to BigNumber
std::map<BigNumber, unsigned int> finalNumber;

// return true if x is a Lychrel number, stop after maxIterations (and return true if "indecisive")
bool isLychrel(unsigned int x, unsigned int maxIterations)
{
  // split integer into its digit, store each digit in a separate cell
  BigNumber number;
  while (x > 0)
  {
    number.push_back(x % 10);
    x /= 10;
  }

  // try to find a palindrome in the first 60 iterations
  for (unsigned int iteration = 0; iteration < maxIterations; iteration++)
  {
    auto reverse = number;
    std::reverse(reverse.begin(), reverse.end());

    // check if palindrome
#ifdef ORIGINAL
    if (iteration > 0) // originally, the initial number is allowed to be a palindrome
#endif
    // no, can't be a Lychrel number
    if (number == reverse)
    {
      finalNumber[number]++;
      return false;
    }

    // add the reverse
    auto sum = number;
    unsigned int carry = 0;
    for (size_t digit = 0; digit < number.size(); digit++)
    {
      // get digit "from the other end"
      sum[digit] += reverse[digit] + carry;

      // overflow ?
      if (sum[digit] >= 10)
      {
        sum[digit] -= 10;
        carry = 1;
      }
      else
      {
        // add one more digit
        carry = 0;
      }
    }
    if (carry > 0)
      sum.push_back(carry);

    number = std::move(sum);
  }

  // yes, we have a Lychrel number
  return true;
}

int main()
{
  // consider a number to be a Lychrel number if no palindrome after that many iterations
#ifdef ORIGINAL
  unsigned int iterations = 50;
#else
  unsigned int iterations = 60;
#endif

  unsigned int maxNumber = 10000;
  std::cin >> maxNumber;

  // count all Lychrel number
  unsigned int count = 0;
  for (unsigned int i = 0; i <= maxNumber; i++)
    if (isLychrel(i, iterations))
      count++;

#ifdef ORIGINAL
  std::cout << count << std::endl;

#else

  unsigned int bestCount = 0;
  BigNumber    bestNumber;
  // find number most converged to
  for (auto f : finalNumber)
    if (bestCount < f.second)
    {
      bestCount  = f.second;
      bestNumber = f.first;
    }

  // print single digits, highest digits first (they were stored last in BigNumber)
  std::reverse(bestNumber.begin(), bestNumber.end());
  for (auto digit : bestNumber)
    std::cout << digit;
  std::cout << " " << bestCount << std::endl;
#endif

  return 0;
}
