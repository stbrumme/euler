// ////////////////////////////////////////////////////////
// # Title
// Roman numerals
//
// # URL
// https://projecteuler.net/problem=89
// http://euler.stephan-brumme.com/89/
//
// # Problem
// For a number written in Roman numerals to be considered valid there are basic rules which must be followed.
// Even though the rules allow some numbers to be expressed in more than one way there is always a "best" way of writing a particular number.
//
// For example, it would appear that there are at least six ways of writing the number sixteen:
//
// ''IIIIIIIIIIIIIIII''
// ''VIIIIIIIIIII''
// ''VVIIIIII''
// ''XIIIIII''
// ''VVVI''
// ''XVI''
//
// However, according to the rules only ''XIIIIII'' and ''XVI'' are valid, and the last example is considered to be the most efficient, as it uses the least number of numerals.
//
// The 11K text file, [roman.txt](https://projecteuler.net/project/resources/p089_roman.txt) (right click and 'Save Link/Target As...'), contains one thousand numbers written in valid, but not necessarily minimal, Roman numerals;
// see [About... Roman Numerals](https://projecteuler.net/about=roman_numerals) for the definitive rules for this problem.
//
// Find the number of characters saved by writing each of these in their minimal form.
//
// Note: You can assume that all the Roman numerals in the file contain no more than four consecutive identical units.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// There are two functions:
// - ''roman2number'' takes a valid Roman number and converts it to an integer
// - ''number2roman'' converts an integer to a minimal ("optimal") Roman number
//
// My program reads all Roman numbers, converts them to integers and back to an optimal Roman number.
// The difference of the strings' lengths is what I'm looking for.
//
// ''roman2number'' reads the Roman numbers backwards:
// - if the current letter is smaller than the previous (its right neighbor) then the current letter must be subtracted else added
// - my code can subtract multiple identical letters, too, e.g. 8 = ''IIX'' which is shorter than ''VIII''
//
// ''number2roman'' has a list of conversion rules to convert an integer to a Roman number.
// Each rule is identified by a number and that rule is applied as long as the integer is larger or equal.
// Whenever ''rules[i]'' applies, one or two letters ''action[i]'' are added to the ''result'':
// ||    4   ||       6      ||       6       ||
// ||  ''i'' || ''rules[i]'' || ''action[i]'' ||
// ||    0   ||     1000     ||     ''M''     ||
// ||    1   ||      900     ||    ''CM''     ||
// ||    2   ||      500     ||     ''D''     ||
// ||    3   ||      400     ||    ''CD''     ||
// ||    4   ||      100     ||     ''C''     ||
// ||    5   ||       90     ||    ''XC''     ||
// ||    6   ||       50     ||     ''L''     ||
// ||    7   ||       40     ||    ''XL''     ||
// ||    8   ||       10     ||     ''X''     ||
// ||    9   ||        9     ||    ''IX''     ||
// ||   10   ||        5     ||     ''V''     ||
// ||   11   ||        4     ||    ''IV''     ||
// ||   12   ||        1     ||     ''I''     ||
//
// # Note
// The problem can be solved by simple search'n'replace, too.
// However, I liked the challenge to write a basic parser for Roman numbers.
//
// # Hackerrank
// I have to print the optimized Roman numbers instead of finding the sum of the length differences.

#include <iostream>
#include <string>

// convert valid roman numbers to binary numbers
unsigned int roman2number(const std::string& roman)
{
  unsigned int result = 0;

  // remember the value of the previous Roman letter
  unsigned int last = 0;
  // true, if the current letter is subtracted (and the next identical letters)
  bool subtract = false;

  // walk through the whole string from the end to the beginning ...
  for (auto i = roman.rbegin(); i != roman.rend(); i++)
  {
    unsigned int current = 0;
    switch (*i)
    {
    case 'M': current = 1000; break;
    case 'D': current =  500; break;
    case 'C': current =  100; break;
    case 'L': current =   50; break;
    case 'X': current =   10; break;
    case 'V': current =    5; break;
    case 'I': current =    1; break;
    }

    // smaller than its right neighbor ? => we must subtract
    if (current < last)
    {
      subtract = true;
      last = current;
    }
    // bigger than its right neighbor ? => we must add
    else if (current > last)
    {
      subtract = false;
      last = current;
    }

    // note: if current == last then we keep the variables "subtract" and "last" in their current state

    // add/subtract accordingly
    if (subtract)
      result -= current;
    else
      result += current;
  }

  return result;
}

std::string number2roman(unsigned int number)
{
  // apply these rules in the presented order:
  // - as long as number >= steps[i] add roman[i] to result
  const unsigned int NumRules = 13;
  const unsigned int rules[NumRules] =
    { 1000,  900, 500,  400, 100,  90,   50,   40,  10,    9,   5,    4,  1  };
  const char* action[NumRules] =
    {  "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };

  // apply all rules ...
  std::string result;
  for (unsigned int i = 0; i < NumRules; i++)
    // ... as often as needed
    while (number >= rules[i])
    {
      // reduce integer
      number -= rules[i];
      // add letter(s)
      result += action[i];
    }

  return result;
}

int main()
{
  // letters saved by optimization
  unsigned int saved = 0;

  unsigned int tests = 1000;
//#define ORIGINAL
#ifndef ORIGINAL
  std::cin >> tests;
#endif

  while (tests--)
  {
    // read Roman number
    std::string roman;
    std::cin >> roman;

    // convert it to an integer and back to an optimal Roman number
    auto number    = roman2number(roman);
    auto optimized = number2roman(number);

    // count how many character were saved
    saved += roman.size() - optimized.size();

#ifndef ORIGINAL
    // print Roman number
    std::cout << optimized << std::endl;
#endif
  }

#ifdef ORIGINAL
  std::cout << saved << std::endl;
#endif
  return 0;
}
