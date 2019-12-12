// ////////////////////////////////////////////////////////
// # Title
// Coded triangle numbers
//
// # URL
// https://projecteuler.net/problem=42
// http://euler.stephan-brumme.com/42/
//
// # Problem
// The nth term of the sequence of triangle numbers is given by, `t_n = frac{n(n+1)}{2}`; so the first ten triangle numbers are:
//
// 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, ...
//
// By converting each letter in a word to a number corresponding to its alphabetical position and adding these values we form a word value.
// For example, the word value for SKY is 19 + 11 + 25 = 55 = `t_10`. If the word value is a triangle number then we shall call the word a triangle word.
//
// Using [words.txt](https://projecteuler.net/project/resources/p042_words.txt) (right click and 'Save Link/Target As...'), a 16K text file containing nearly two-thousand common English words, how many are triangle words?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My function ''getTriangle'' returns either ''NoTriangle = -1'' or the triangle index if the parameter ''x'' is a triangle number.
// E.g. ''getTriangle(55) = 10''.
//
// I use my own approximation formula for the triangle index which can be derived as follows:
// A triangle number `t_n` is a defined as
// `t_n = n (n + 1) / 2`
//
// Ff `x` is such a triangle number `t_n` then
// ` x = n (n + 1) / 2`
// `2x = n (n + 1)`
// `2x = n^2 + n`
//
// For any `a^2` we know for `a`'s successor
// `(a + 1)^2 = a^2 + 2a + 1`
//
// Therefore the relationship:
// `n^2 <  n^2 + n < n^2 + 2n + 1`
// `n^2 <    2x    < (n + 1)^2`
// ` n  < sqrt{2x} < (n + 1)^2`
//
// In order to find the triangle index `n`, I just compute `n = sqrt{2x}`.
// If `t_n = n (n + 1) / 2 = x` then `n` is indeed what we were looking for - else the function returns ''-1''.
//
// # Note
// Solving the quadratic equation leads to the same result:
// `t_n = x`
// `x = n (n + 1) / 2`
// `x = (n^2 + n) / 2`
// `2x = n^2 + n`
// `0 = n^2 + n - 2x`
//
// `n` must be positive so only one solution is left:
// `n = frac{sqrt{1 + 8x} - 1}{2}`
//
// In the end you have to check this result as well: only if it an integer then `n` is our result.
//
// Project Euler's file can be easily parsed in C++.
// Initially I included it in my source code (which works flawlessly) but then decided to read from STDIN.
//
// # Hackerrank
// I just have to check numbers whether they are triangle numbers or not.
// The major problem was that input values can be up to `10^18`.

#include <string>
#include <iostream>
#include <cmath>

const int NoTriangle = 0;
// return triangle index or -1 if not a triangle number
int getTriangle(unsigned long long x)
{
  unsigned long long n = sqrt(2*x);

  // if n it truely the right answer then t(n) = x
  unsigned long long check = n * (n + 1) / 2;
  if (x == check)
    return n;
  else
    return NoTriangle;
}

// read a single word from STDIN, syntax: "abc","def","xyz"
std::string readWord()
{
  std::string result;
  while (true)
  {
    // read one character
    char c = std::cin.get();
    // no more input ?
    if (!std::cin)
      break;

    // ignore quotes
    if (c == '"')
      continue;
    // finish when a comma appears
    if (c == ',')
      break;

    // nope, just an ordinary letter (no further checks whether c in 'A'..'Z')
    result += c;
  }
  return result;
}

int main()
{
//#define ORIGINAL
#ifdef ORIGINAL

  unsigned int triangleWords = 0;
  while (true)
  {
    // read next word
    auto word = readWord();
    if (word.empty())
      break;

    unsigned int sum = 0;
    // A = 1, B = 2, ...
    for (auto c : word)
      sum += c - 'A' + 1; // all words contain only uppercase letters without spaces or other characters

    // another "triangle word" ?
    if (getTriangle(sum) != NoTriangle)
      triangleWords++;
  }
  std::cout << triangleWords << std::endl;

#else

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    // all work is done in getTriangle()
    unsigned long long x;
    std::cin >> x;
    std::cout << getTriangle(x) << std::endl;
  }
#endif

  return 0;
}
