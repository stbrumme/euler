// ////////////////////////////////////////////////////////
// # Title
// Odd elimination
//
// # URL
// https://projecteuler.net/problem=539
// http://euler.stephan-brumme.com/539/
//
// # Problem
// Start from an ordered list of all integers from 1 to `n`.
// Going from left to right, remove the first number and every other number afterward until the end of the list.
// Repeat the procedure from right to left, removing the right most number and every other number from the numbers left.
// Continue removing every other numbers, alternating left to right and right to left, until a single number remains.
//
// Starting with `n = 9`, we have:
// __1__ 2 __3__ 4 __5__ 6 __7__ 8 __9__
// 2 __4__ 6 __8__
// __2__ 6
// 6
//
// Let `P(n)` be the last number left starting with a list of length `n`.
// Let `S(n) = sum_{k=1}^n{P(k)}`
//
// You are given `P(1)=1`, `P(9) = 6`, `P(1000)=510`, `S(1000)=268271`.
//
// Find `S(10^18) mod 987654321`.
//
// # Written by
// Stephan Brumme
// October 2017
//
// # Idea
// The brute-force code to find ''P(n)'' filters an array and returns the last number (see ''bruteForceP'').
// ''bruteForceS'' is a simple loop and calls ''bruteForceP''.
// These two functions can solve `S(1000)` quite fast but anything beyond `n = 10^5` would take forever.
//
// Looking at the values produces by ''bruteForceP'' I spotted a pattern after a while:
// - two consecutive numbers `P(x)` and `P(x+1)` are always `P(x)=P(x+1)` if `x` is even (it's a multiple of 2)
// - two consecutive numbers `P(x)` and `P(x+2)` are always `P(x)=P(x+2)-2` if `x` is a multiple of 4
// - combining both: `P(x+1) = P(x+3) - 2` if `x` is a multiple of 4
// - for whatever reason `P(2)` is an exception
//
// The first values of `P(n)` are:
// ||   3    || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 ||  2 ||  2 ||  2 ||  2 ||  2 ||  2 ||  2 ||
// ||! `n`   ++ 1 ++ 2 ++ 3 ++ 4 ++ 5 ++ 6 ++ 7 ++ 8 ++ 9 ++ 10 ++ 11 ++ 12 ++ 13 ++ 14 ++ 15 ++ 16 ||
// || `P(n)` ++ 1 ++ 2 ++ 2 ++ 2 ++ 2 ++ 4 ++ 4 ++ 6 ++ 6 ++  8 ++  8 ++  6 ++  6 ++  8 ++  8 ++  6 ||
//
// Even more interesting, `P(x) = 4 * P(x div 4)` if `x-2` or `x-3` is a multiple of 4.
// If `x` or `x-1` is a multiple of 4 then I have to subtract 2.
//
// E.g. `P(7)  = 4 * P( 7 div 4)     = 4 * P(1)     = 4 * 1 = 4`
// and  `P(12) = 4 * P(12 div 4) - 2 = 4 * P(3) - 2 = 4 * 2 - 2 = 6`
//
// Now I can easily compute `P(10^18)` but `S(10^18)` would still require a loop with `10^18` iterations.
// It's easy to compute 4 numbers at once, but it doesn't help much because `S(10^18)` is still out of reach.
// `S(10^8)` is found in a few seconds and OpenMP might help me to solve even `S(10^10)` in under a minute.
//
// I think that computing more than four numbers at once is possible but can't deduce a nice pattern.
// So far my solution was based on a "lucky" discovery and I'm afraid that I don't get lucky twice on the same problem.

#include <iostream>
#include <vector>

// find last number standing ... between 1 and n
unsigned int bruteForceP(unsigned int n)
{
  std::vector<unsigned int> numbers;
  // fill with 1..limit
  for (unsigned int i = 1; i <= n; i++)
    numbers.push_back(i);

  bool left2right = true;
  while (numbers.size() > 1)
  {
    std::vector<unsigned int> next;

    // from left to right: keep numbers at odd positions (1,3,5,...), note: zero-based index
    // from right to left: if there is an odd amount of numbers, keep odd numbers, else even numbers
    size_t i = left2right ? 1 : numbers.size() % 2;
    for (; i < numbers.size(); i += 2)
      next.push_back(numbers[i]);

    // ... and the other direction in the next iteration
    left2right = !left2right;
    numbers = std::move(next);
  }

  return numbers[0];
}

// sum of bruteForceP(1..limit)
// solves S(10^5) in a few seconds
unsigned int bruteForceS(unsigned int limit)
{
  unsigned int sum = 0;
  for (unsigned int i = 1; i <= limit; i++)
    sum += bruteForceP(i);
  return sum;
}

// there is a pattern:
// P(n) = 4 * P(n/4) - 2 if n % 4 = {0,1}
// P(n) = 4 * P(n/4)     if n % 4 = {2,3}
// and the initial values are P(1) = 1, P(2) = P(3) = 2
unsigned long long fastP(unsigned long long n)
{
  // it's pretty fast to solve P() for small n with brute force
  const size_t CacheSize = 20;
  static unsigned char cache[CacheSize] = { 0 };
  if (n <= CacheSize)
  {
    // actually I could hard-wire 1 => 1, 2 => 2, 3 => 2, 4 => 2
    // but that would render my nice brute-force code useless ;-)
    if (cache[n] == 0)
      cache[n] = bruteForceP(n);
    return cache[n];
  }

  auto result   = fastP(n / 4) * 4;
  // keep result unchanged if second-to-last bit is set (same as n % 4 >= 2)
  return result + (n & 2) - 2; // branch-free code of if (n%4>=2) return result; else return result - 2;
}

// right now almost the same code as bruteForceS, but calls fastP instead of bruteForceP and respects the modulo
// solves S(10^8) in a few seconds
unsigned long long slowS(unsigned long long limit, unsigned int modulo = 987654321)
{
  unsigned long long sum = 0;
  for (unsigned long long i = 1; i <= limit; i++)
  {
    auto current = fastP(i);
    sum += current;
    //std::cout << i << ": +" << current << " =" << sum << std::endl;

    // avoid too frequent modulo operations
    if (sum > 10 * modulo)
      sum %= modulo;
  }
  return sum % modulo;
}

// no idea yet how to accelerate slowS ...
unsigned long long fastS(unsigned long long /*limit*/, unsigned int /*modulo = 987654321*/)
{
  return 0;
}

int main()
{
  unsigned long long limit = 1000000000000000000ULL;
  std::cin >> limit;

  // solve examples
  //std::cout << bruteForceP(9) << std::endl;
  //std::cout << bruteForceS(1000) << std::endl;

  std::cout << slowS(limit) << std::endl;
  return 0;
}
