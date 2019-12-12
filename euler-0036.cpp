// ////////////////////////////////////////////////////////
// # Title
// Double-base palindromes
//
// # URL
// https://projecteuler.net/problem=36
// http://euler.stephan-brumme.com/36/
//
// # Problem
// The decimal number, 585 = 10010010012 (binary), is palindromic in both bases.
// Find the sum of all numbers, less than one million, which are palindromic in base 10 and base 2.
//
// (Please note that the palindromic number, in either base, may not include leading zeros.)
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My short function ''num2str'' converts a number ''x'' into a ''std::string'' with ''base''.
// It's more or less the same code I used in previous problems when I wanted to extract right-most digit of a number.
// Note that the result "grows" from right-to-left, that's why I always insert a single char at position 0.
//
// The function ''isPalindrome'' uses the STL function ''std::reverse'' to efficiently reverse a string in-place.
// If it's identical to the original string, then we have a palindrome.
//
// The main loop checks number with base 10 first, because they are shorter than numbers with base 2 to 9.
// In most cases we can abort earlier and save a little time.
//
// # Note
// Of course you can speed up the whole process considerably:
// 1. generate all numbers from ''1'' to ''y'' where ''y'' has half as many digits as ''x''.
// 2. then convert to an ''std::string'' and append its reverse.
// 3. convert that string to an integer ''x'' and test with ''isPalindrome(num2str(x, 2))'' whether ''x'' 's binary representation is a palindrome, too.
//
// This way you have to check 1000 instead of 1000000 numbers.
//
// I haven't benchmark it but my suspicion is that if ''num2str'' appends digits (instead of prepending) and reverses the string as a final step,
// then we might see a speed-up as well.
//
// # Hackerrank
// Base can be anything from 2 to 9.

#include <iostream>
#include <string>
#include <algorithm>

// base: decimal=10, binary=2
std::string num2str(unsigned int x, unsigned int base)
{
  std::string result;
  while (x > 0)
  {
    auto digit = x % base;
    x         /= base;
    result.insert(0, 1, char(digit + '0'));
  }
  return result;
}

// true if string is a palindrome
bool isPalindrome(const std::string& s)
{
  auto other = s;
  std::reverse(other.begin(), other.end());
  return other == s;
}

// I was quite surprised that the basic brute-force approach is sufficiently fast ...
int main()
{
  unsigned int limit, base; // limit = 1000000 and base = 2
  std::cin >> limit >> base;

  unsigned int sum = 0;
  for (unsigned int x = 1; x < limit; x++)
    if (isPalindrome(num2str(x, 10)) &&  // palindrome in base 10 ?
        isPalindrome(num2str(x, base)))  // palindrome in base k ?
      // yes, double palindrome
      sum += x;

  std::cout << sum << std::endl;
  return 0;
}
