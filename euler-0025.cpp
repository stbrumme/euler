// ////////////////////////////////////////////////////////
// # Title
// 1000-digit Fibonacci number
//
// # URL
// https://projecteuler.net/problem=25
// http://euler.stephan-brumme.com/25/
//
// # Problem
// The Fibonacci sequence is defined by the recurrence relation:
//
// `F_n = F_{n-1} + F_{n-2}`, where `F_1 = 1` and `F_2 = 1`.
//
// Hence the first 12 terms will be:
//
// `F_1 = 1`
// `F_2 = 1`
// `F_3 = 2`
// `F_4 = 3`
// `F_5 = 5`
// `F_6 = 8`
// `F_7 = 13`
// `F_8 = 21`
// `F_9 = 34`
// `F_{10} = 55`
// `F_{11} = 89`
// `F_{12} = 144`
//
// The 12th term, `F_{12}`, is the first term to contain three digits.
//
// What is the index of the first term in the Fibonacci sequence to contain 1000 digits?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// I precompute all Fibonacci number with up to 5000 digits (a design decision influenced by Hackerrank's modified problem) and keep those results in ''cache''.
//
// Unfortunately, there is a small problem with C++ ...
// `F_{47}=2971215073` is the largest Fibonacci number that fits in a 32-bit integer and
// `F_{94}=19740274219868223167` is too big for a 64-bit integer.
//
// My program stores such large number as a ''std::vector'' where index 0 contains the least significant digit ("in reverse order").
// E.g. `F_{23}=28657` is represented as ''{ 7, 5, 6, 8, 2 }''
//
// The function ''add'' returns the sum of two large numbers ''a'' and ''b'' where ''b>=a''.
// The algorithm behind this function is exactly the same you were taught in primary school.
//
// # Alternative
// The main problem was adding two very large numbers. When programming in Python, Java, etc. you get these things for free.
//
// # Hackerrank
// The large amount of test cases was the main cause for dividing my solution into two parts;
// 1. precompute all relevant Fibonacci numbers (done once - "expensive")
// 2. look up the result (performed many, many times - "cheap")

#include <vector>
#include <iostream>

// store single digits because numbers get too big for C++
typedef std::vector<unsigned int> Digits;

// Hackerrank's upper limit
const unsigned int MaxDigits = 5000;

// add two long number where b >= a
Digits add(const Digits& a, const Digits& b)
{
  Digits result = b;

  unsigned int carry = 0;
  for (unsigned int i = 0; i < result.size(); i++)
  {
    // "a" might have less digits than "b"
    if (i < a.size())
      result[i] += a[i];

    // don't forget about the carry ...
    result[i] += carry;

    // handle overflow
    if (result[i] >= 10)
    {
      carry = 1;
      result[i] -= 10;
    }
    else
      carry = 0;
  }

  // largest digit not overflowing ?
  if (carry != 0)
    result.push_back(carry);

  return result;
}

int main()
{
  // precompute number of steps we needed for each number of digits
  // [number of digits] => [index of smallest Fibonacci number]
  std::vector<unsigned int> cache = { 0, 1 }; // F_0 is undefined
  cache.reserve(MaxDigits);

  // f(1) = 1
  Digits a = { 1 };
  // f(2) = 1
  Digits b = { 1 };
  // we have predefined F_1 and F_2
  unsigned int fiboIndex = 2;

  while (cache.size() <= MaxDigits)
  {
    // next Fibonacci number
    fiboIndex++;
    auto next = add(a, b);
    a = std::move(b);
    b = std::move(next);

    // digits of current Fibonacci number
    auto numDigits = b.size();
    // digits of the previously largest Fibonacci number
    auto largestKnown = cache.size() - 1; // don't count the 0th element

    // one more digit than before ?
    if (largestKnown < numDigits)
      cache.push_back(fiboIndex);
  }

  // simply look up the result
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int numDigits;
    std::cin >> numDigits;
    std::cout << cache[numDigits] << std::endl;
  }

  return 0;
}
