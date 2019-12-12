// ////////////////////////////////////////////////////////
// # Title
// Bouncy numbers
//
// # URL
// https://projecteuler.net/problem=112
// http://euler.stephan-brumme.com/112/
//
// # Problem
// Working from left-to-right if no digit is exceeded by the digit to its left it is called an increasing number; for example, 134468.
// Similarly if no digit is exceeded by the digit to its right it is called a decreasing number; for example, 66420.
// We shall call a positive integer that is neither increasing nor decreasing a "bouncy" number; for example, 155349.
// Clearly there cannot be any bouncy numbers below one-hundred, but just over half of the numbers below one-thousand (525) are bouncy.
// In fact, the least number for which the proportion of bouncy numbers first reaches 50% is 538.
//
// Surprisingly, bouncy numbers become more and more common and by the time we reach 21780 the proportion of bouncy numbers is equal to 90%.
//
// Find the least number for which the proportion of bouncy numbers is exactly 99%.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// ''isBouncy'' determines whether its parameter is bouncy or not by stepping through its digits.
// The rest is just a brute-force search.
//
// # Hackerrank
// Any percentage can be entered.
// Even though my brute-force search finds the solution to the original problem (99%) in less than 0.1 seconds, it's way too slow for
// the potentially huge search space of Hackerrank. Someone indicated that one "answer requires about 90 bits".

#include <iostream>

// return true if x is a bouncy number
bool isBouncy(unsigned long long x)
{
  // figure out whether x is monotonic ascending or descending
  // it's bouncy if neither ascending nor descending
  bool ascending  = true;
  bool descending = true;

  // initial digit (the right-most digit)
  auto previous = x % 10;
  x /= 10;

  // still digits left ?
  while (x > 0)
  {
    // current digit
    auto current = x % 10;

    // compare two digits
    descending &= previous >= current;
    ascending  &= previous <= current;

    // bouncy ?
    if (!ascending && !descending)
      return true;

    // keep going ...
    x /= 10;
    previous = current;
  }

  // not bouncy (either ascending, descending or all digits are equal)
  return false;
}


int main()
{
  unsigned int tests = 1;
  std::cin >> tests;

  while (tests--)
  {
    // original problem: 99%
    unsigned long long p =  99;
    unsigned long long q = 100;
    std::cin >> p >> q;

    // brute-force ...
    unsigned long long current   = 100; // no bouncy numbers below 100
    unsigned long long numBouncy =   0;
    do
    {
      // check next number if bouncy
      current++;
      if (isBouncy(current))
        numBouncy++;
    } while (numBouncy * q < current * p); // same as numBouncy/current == p/q (=99%)

    // print result
    std::cout << current << std::endl;
  }

  return 0;
}
