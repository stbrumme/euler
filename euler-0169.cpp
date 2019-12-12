// ////////////////////////////////////////////////////////
// # Title
// Exploring the number of different ways a number can be expressed as a sum of powers of 2
//
// # URL
// https://projecteuler.net/problem=169
// http://euler.stephan-brumme.com/169/
//
// # Problem
// Define `f(0)=1` and `f(n)` to be the number of different ways `n` can be expressed as a sum of integer powers of 2 using each power no more than twice.
//
// For example, `f(10)=5` since there are five different ways to express 10:
//
// 1 + 1 + 8
// 1 + 1 + 4 + 4
// 1 + 1 + 2 + 2 + 4
// 2 + 4 + 4
// 2 + 8
//
// What is `f(10^25)`?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// It's easy to write a recursive function to solve the problem for small input, such as 10.
// Unfortunately it becomes very slow when analyzing number larger than `10^6`. Memoization helps a bit but can't find the solution for `10^25`, though.
//
// I created a small list of ''solve(1..50)'' and noticed a certain pattern:
// ''solve(2^y) = y + 1'', e.g. ''solve(16) = solve(2^4) = 5''
// ''solve(2^y - 1) = 1'', e.g. ''solve(31) = solve(2^5 - 1) = 1''
//
// If the binary representation consists of 1s only, then the result is always ''1''.
// If the binary representation consists of a single 1 followed by ''y'' zeros, then the result is always ''y+1''.
//
// The whole thing got even more interesting:
// appending 1s doesn't change the result, too: ''solve(9) = solve(9 >> 1) = solve(4) = 3''
//
// And after fiddling around with the binary representations, I found that the number of consecutive zeros plays a major role.
// I came up with an iterative formula:
// `f_i(x) = f_{i-1}(x) + zeros_i(x) * sum_{k=1..i}{f_k(x)}`
// where `zeros_i(x)` stands for the number of zeros between two 1s, beginning from the left side of the binary representation of `x`.
//
// ''countZeros'' returns a vector of those lengths of zero bits, e.g. `100000_10 = 11000011010100000_2` ==> ''{ 0, 4, 0, 1, 1, 5 }''
// If x is odd, then the right-most 1s have to be removed until x becomes even.
//
// # Note
// After finishing my program, I discovered that this is called the [Diatomic Series](http://mathworld.wolfram.com/SternsDiatomicSeries.html) - a term haven't heard of before.
// This program doesn't compile with Visual C++ because I had to use the GCC extension ''__int128'' since `10^25 > 2^64`.

#include <iostream>
#include <vector>
#include <string>

// GCC-specific 128 bit integer
typedef unsigned __int128 BigNum;

// brute-force solver, not used anymore
unsigned int solve(unsigned int x, unsigned int minAdd = 1)
{
  if (x == 0)
    return 1;

  unsigned long long result = 0;
  auto current = minAdd;
  while (current <= x)
  {
    // try to remove current 2^k once
    result += solve(x - current, current*2);
    // and twice
    if (x >= 2*current)
      result += solve(x - 2*current, current*2);

    // process 2^(k+1)
    current *= 2;
  }

  return result;
}

// count consecutive zeros in binary representation
std::vector<unsigned int> countZeros(BigNum x)
{
  std::vector<unsigned int> result;

  // right-most 1s don't affect the result, remove them
  while ((x & 1) == 1)
    x >>= 1;

  auto consecutive = 0;
  while (x > 0)
  {
    // look at right-most bit
    if ((x & 1) == 0)
    {
      // once more zero
      consecutive++;
    }
    else
    {
      // runs of consecutive zeros interrupted by a 1
      result.insert(result.begin(), consecutive);
      consecutive = 0;
      // note: consecutive can be zero, too !
    }

    x >>= 1;
  }

  return result;
}

int main()
{
  // can't read anything >= 2^64, must do it via string parsing
  std::string large;
  std::cin >> large;
  BigNum x = 0;
  // digit by digit ...
  for (auto c : large)
    x = x*10 + (c - '0');

  // count consecutive zeros in binary representation
  auto zeros = countZeros(x);

  // compute number of distinct representations
  unsigned long long result = 1;
  unsigned long long sum    = 1;
  for (unsigned int i = 0; i < zeros.size(); i++)
  {
    result += zeros[i] * sum;
    sum    += result;
  }
  std::cout << result << std::endl;
  return 0;
}
