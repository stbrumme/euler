// ////////////////////////////////////////////////////////
// # Title
// Sub-string divisibility
//
// # URL
// https://projecteuler.net/problem=43
// http://euler.stephan-brumme.com/43/
//
// # Problem
// The number, 1406357289, is a 0 to 9 pandigital number because it is made up of each of the digits 0 to 9 in some order,
// but it also has a rather interesting sub-string divisibility property.
//
// Let `d_1` be the 1st digit, `d_2` be the 2nd digit, and so on. In this way, we note the following:
//
// `d_2 d_3 d_4 = 406` is divisible by 2
// `d_3 d_4 d_5 = 063` is divisible by 3
// `d_4 d_5 d_6 = 635` is divisible by 5
// `d_5 d_6 d_7 = 357` is divisible by 7
// `d_6 d_7 d_8 = 572` is divisible by 11
// `d_7 d_8 d_9 = 728` is divisible by 13
// `d_8 d_9 d_10 = 289` is divisible by 17
//
// Find the sum of all 0 to 9 pandigital numbers with this property.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Once more, ''std::next_permutation'' turns out to be a rather handy feature:
// I generate all permutations of ''pan = "0123456789"'' and check all its substrings with length 3 for divisibility with the first prime numbers.
//
// ''str2num'' converts an ASCII string to a number, ignoring leading zeros: ''str2num("012") = 12''
//
// We need only the first 7 prime numbers - that's why I opted against a full prime sieve and just declared a precomputed array ''primes''.
//
// # Note
// Ten digits can exceed 32 bits, therefore you'll find ''unsigned long long'' instead of ''unsigned int'' in a few places.
//
// There are many optimization possible: when applying the rules of divisibility by 3, 5, ... then
// we could easily exclude the majority of permutations. Nevertheless, the non-optimized code runs fast enough.
//
// # Hackerrank
// The number of digits can vary. All I do is adjusting ''pan'': anything else remains the same.
// Hackerrank accepts pandigital numbers that start with a zero.

#include <string>
#include <iostream>
#include <algorithm>

// convert a string to a number
unsigned long long str2num(const std::string& x)
{
  // process string from left to right
  unsigned long long result = 0;
  for (auto c : x)
  {
      // shift digits
      result *= 10;
      // add new digit on the right-hand side
      result += c - '0'; // was ASCII
  }
  return result;
}

int main()
{
  // available digits
  std::string pan = "0123456789"; // unlike other problems, zero is allowed this time

  // remove a few digits if test case requires this
  unsigned int maxDigit;
  std::cin >> maxDigit;
  pan.erase(maxDigit + 1);

  // all divisors
  const unsigned int primes[] = { 2,3,5,7,11,13,17 };

  // result
  unsigned long long sum = 0;

  // look at all permutations
  do
  {
    // let's assume it's a great number ;-)
    bool ok = true;

    // check each 3-digit substring for divisibility
    for (unsigned int i = 0; i + 2 < maxDigit; i++)
    {
      // check pan[1..3] against primes[0],
      // check pan[2..4] against primes[1],
      // check pan[3..5] against primes[2] ...
      std::string check = pan.substr(i + 1, 3);
      if (str2num(check) % primes[i] != 0)
      {
        // nope ...
        ok = false;
        break;
      }
    }

    // passed all tests, it's great indeed !
    if (ok)
      sum += str2num(pan);
  } while (std::next_permutation(pan.begin(), pan.end()));

  std::cout << sum << std::endl;
  return 0;
}
