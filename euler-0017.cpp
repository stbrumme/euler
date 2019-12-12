// ////////////////////////////////////////////////////////
// # Title
// Number letter counts
//
// # URL
// https://projecteuler.net/problem=17
// http://euler.stephan-brumme.com/17/
//
// # Problem
// If the numbers 1 to 5 are written out in words: one, two, three, four, five, then there are 3 + 3 + 5 + 4 + 4 = 19 letters used in total.
// If all the numbers from 1 to 1000 (one thousand) inclusive were written out in words, how many letters would be used?
//
// __NOTE:__ Do not count spaces or hyphens. For example, 342 (three hundred and forty-two) contains 23 letters
// and 115 (one hundred and fifteen) contains 20 letters. The use of "and" when writing out numbers is in compliance with British usage.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My program converts a number into its "written" representation because of the Hackerrank requirements (see below).
// A simple loop from 1 to 1000 creates a ton of string and computes the sums their lengths.
//
// The function ''convert'' immediately returns the name of numbers in `[0;19]`.
// For all other numbers it calls itself recursively:
// e.g. when the parameter ''x'' is in `[20;99]` then its higher digit is converted directly into a word, its lower is found by a recursive call
//
// My code is a bit bloated because of spelling differences between Project Euler and Hackerrank.
// I had to be a bit careful not to call the function ''convert'' recursively with parameter zero.
//
// # Note
// Rules for finding the English names of numbers have far less exceptions than the German rules ...
//
// # Alternative
// The original problem can be solved by just counting the letter without actually "building" the names, too.
//
// # Hackerrank
// The Hackerrank problem is quite different: you have to convert a number into its name.
// Their spelling rules vary, too.
//
// __Note:__ Unlike most of my other programs, ''#define ORIGINAL'' is not active in the source code listing due to interactive tests.

#include <iostream>
#include <string>

// customize code for original problem
//#define ORIGINAL

// convert a number into its written representation
std::string convert(unsigned long long x)
{
#ifdef ORIGINAL
  const std::string Gap = " And "; // British
  const std::string ConnectTensAndOnes = "-";
#else
  const std::string Gap = " ";
  const std::string ConnectTensAndOnes = " ";
#endif

  // none-composite names
  switch(x)
  {
    case  0: return "Zero";
    case  1: return "One";
    case  2: return "Two";
    case  3: return "Three";
    case  4: return "Four";
    case  5: return "Five";
    case  6: return "Six";
    case  7: return "Seven";
    case  8: return "Eight";
    case  9: return "Nine";
    case 10: return "Ten";
    case 11: return "Eleven";
    case 12: return "Twelve";
    case 13: return "Thirteen";
    case 14: return "Fourteen";
    case 15: return "Fifteen";
    case 16: return "Sixteen";
    case 17: return "Seventeen";
    case 18: return "Eighteen";
    case 19: return "Nineteen";
    default: break;
  }

  // two-digit composite names
  if (x >= 20 && x < 100)
  {
    auto ones = x % 10;
    auto tens = x / 10;
    auto strOnes = (ones != 0) ? ConnectTensAndOnes + convert(ones) : "";
    switch (tens)
    {
      case 2: return "Twenty"  + strOnes;
      case 3: return "Thirty"  + strOnes;
      case 4: return "Forty"   + strOnes; // <= often misspelt/misspelled ;)
      case 5: return "Fifty"   + strOnes;
      case 6: return "Sixty"   + strOnes;
      case 7: return "Seventy" + strOnes;
      case 8: return "Eighty"  + strOnes;
      case 9: return "Ninety"  + strOnes;
      default: break; // never reached
    }
  }

  // three-digit composite names
  if (x >= 100 && x < 1000)
  {
    auto onesAndTens = x % 100;
    auto hundreds    = x / 100;
    auto strOnesAndTens = (onesAndTens != 0) ? Gap + convert(onesAndTens) : "";
    return convert(hundreds) + " Hundred" + strOnesAndTens;
  }

  // four to six digits
  if (x >= 1000 && x < 1000000)
  {
    auto low  = x % 1000; // variable names got too long, I went for a generic one ...
    auto high = x / 1000;
    auto strLow = (low != 0) ? Gap + convert(low) : "";
    return convert(high) + " Thousand" + strLow;
  }

  // seven to nine digits
  if (x >= 1000000 && x < 1000000000)
  {
    auto low  = x % 1000000;
    auto high = x / 1000000;
    auto strLow = (low != 0) ? Gap + convert(low) : "";
    return convert(high) + " Million" + strLow;
  }

  // ten to twelve digits
  if (x >= 1000000000 && x < 1000000000000ULL) // careful: must be a 64 bit constant, add "LL"
  {
    auto low  = x % 1000000000;
    auto high = x / 1000000000;
    auto strLow = (low != 0) ? Gap + convert(low) : "";
    return convert(high) + " Billion" + strLow;
  }

  // thirteen to fifteen digits
  if (x >= 1000000000000ULL && x < 1000000000000000ULL)
  {
    auto low  = x % 1000000000000ULL;
    auto high = x / 1000000000000ULL;
    auto strLow = (low != 0) ? Gap + convert(low) : "";
    return convert(high) + " Trillion" + strLow;
  }

  // not reached
  return "?";
}

int main()
{
#ifdef ORIGINAL
  // count number of letters
  unsigned int sum = 0;
  for (unsigned int i = 1; i <= 1000; i++)
  {
    auto name = convert(i);
    for (auto c : name)
      if (std::isalpha(c)) // discard spaces/hyphens/etc.
        sum++;
  }
  std::cout << sum << std::endl;
#else
  // just print several names according to input
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long x;
    std::cin >> x;
    std::cout << convert(x) << std::endl;
  }
#endif
  return 0;
}
