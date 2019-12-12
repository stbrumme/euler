// ////////////////////////////////////////////////////////
// # Title
// Coin partitions
//
// # URL
// https://projecteuler.net/problem=78
// http://euler.stephan-brumme.com/78/
//
// # Problem
// Let `p(n)` represent the number of different ways in which `n` coins can be separated into piles.
// For example, five coins can be separated into piles in exactly seven different ways, so `p(5)=7`.
//
// ''OOOOO''
// ''OOOO   O''
// ''OOO   OO''
// ''OOO   O   O''
// ''OO   OO   O''
// ''OO   O   O   O''
// ''O   O   O   O   O''
//
// Find the least value of `n` for which `p(n)` is divisible by one million.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Brute-forcing the solution for small `n` yielded the sequence:
// 1,2,3,5,7,11,15,122,30,42,56,77,101,...
// I searched the web and found these numbers in OEIS A000041.
//
// After that I read the problem description again and looked up "partition" on Wikipedia: https://en.wikipedia.org/wiki/Partition_%28number_theory%29#Recurrence_formula
// And found a link to Euler's formula which is based on pentagonal numbers: https://en.wikipedia.org/wiki/Pentagonal_number_theorem
// ''result(i) =   result(pentagonal(+1)) + result(pentagonal(-1))''
// ''            - result(pentagonal(+2)) - result(pentagonal(-2))''
// ''            + result(pentagonal(+3)) + result(pentagonal(-3))''
// ''            - result(pentagonal(+4)) - result(pentagonal(-4))''
// ''            ...''
//
// ''result(417)'' is too big for a 64 bit integer (and there is no solution among the first 416 partitions).
// Luckily, we need to find the first number that is divisible by one million, that means where ''result(x) % 1000000 = 0''.
// Hence I store the number of partitions modulo 1000000. Whenever it is zero, my program can abort.
//
// # Hackerrank
// The modified problem asks for the number of partitions of an input value (modulo `10^9 - 7`).
// Results from previous test cases are kept in ''partitions'' to speed up the process.

#include <iostream>
#include <vector>

int main()
{
  // store result (modulo 10^6 or 10^9 + 7)
  std::vector<unsigned long long> partitions;
  // degenerated case, there's one partition for an empty pile
  partitions.push_back(1);

//#define ORIGINAL
#ifdef ORIGINAL
  const long long modulo =    1000000; // 10^6
#else
  const long long modulo = 1000000007; // 10^9 + 7

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
#endif

  {
    unsigned int limit = 100000; // the solution is < 100000, program ab
#ifndef ORIGINAL
    std::cin >> limit;
#endif

    // fill cache
    for (unsigned int n = partitions.size(); n <= limit; n++)
    {
      // sum according to Euler's formula
      long long sum = 0;

      // all pentagonal numbers where pentagonal(i) <= n
      for (unsigned int i = 0; ; i++) // abort inside loop
      {
        // generate alternating numbers +1,-1,+2,-2,+3,-3,...
        int alternate = 1 + (i / 2); // generate the digit 1,1,2,2,3,3,...
        if (i % 2 == 1)
          alternate = -alternate;    // flip the sign for every second number

        // pentagonal index, "how far we go back" in partitions[]
        unsigned int offset = alternate * (3 * alternate - 1) / 2;
        // can't go back that far ? (array index would be negative)
        if (n < offset)
          break;

        // add two terms, subtract two terms, add two terms, subtract two terms, ...
        if (i % 4 < 2)
          sum += partitions[n - offset]; // i % 4 = { 0, 1 }
        else
          sum -= partitions[n - offset]; // i % 4 = { 2, 3 }

        // only the last digits are relevant
        sum %= modulo;

      }

      // note: sum can be temporarily negative
      if (sum < 0)
        sum += modulo;

#ifdef ORIGINAL
      // "divisible by one million" => sum % 1000000 == 0
      // last 6 digits (modulo was 10^6) are zero
      if (sum == 0)
        break;
#endif

      partitions.push_back(sum);
    }

    // print (cached) result
#ifdef ORIGINAL
    std::cout << partitions.size() << std::endl;
#else
    std::cout << partitions[limit] << std::endl;
#endif
  }

  return 0;
}
