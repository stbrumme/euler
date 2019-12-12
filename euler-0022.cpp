// ////////////////////////////////////////////////////////
// # Title
// Names scores
//
// # URL
// https://projecteuler.net/problem=22
// http://euler.stephan-brumme.com/22/
//
// # Problem
// Using [names.txt](https://projecteuler.net/project/resources/p022_names.txt), a 46K text file containing over five-thousand first names, begin by sorting it into alphabetical order.
// Then working out the alphabetical value for each name, multiply this value by its alphabetical position in the list to obtain a name score.
//
// For example, when the list is sorted into alphabetical order, COLIN, which is worth 3 + 15 + 12 + 9 + 14 = 53, is the 938th name in the list.
// So, COLIN would obtain a score of 938 x 53 = 49714.
//
// What is the total of all the name scores in the file?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// When using an ''std::set'' all its elements are automatically sorted.
// A second container, my ''std::map'' named ''sorted'' contains each name as a key and its position in that set.
//
// All names are written in uppercase - and my program doesn't verify it.
// The value of a name is defined as the sum of its letters where A=1, B=2, ... which boils down to ''value += c - 'A' + 1''
//
// # Hackerrank
// The modified Hackerrank differs significantly and is surprisingly easier than the original problem.
//
// # Note
// Project Euler's file can be easily parsed in C++.
// Initially I included it in my source code (which works flawlessly) but then decided to read from STDIN.

#include <iostream>
#include <string>
#include <map>
#include <set>

//#define ORIGINAL

// read a single name from STDIN, syntax: "abc","def","xyz"
std::string readName()
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
  // note: an std::set is always sorted
  std::set<std::string> names;

#ifdef ORIGINAL

  while (true)
  {
    // read a single name, abort when empty
    auto name = readName();
    if (name.empty())
      break;
    names.insert(name);
  }

#else

  unsigned int numNames;
  std::cin >> numNames;
  while (numNames--)
  {
    // Hackerrank's names are separated by a space
    std::string name;
    std::cin >> name;
    // add to our set
    names.insert(name);
  }
#endif

  // walk through all names in alphabetic order, keep track of their position
  // store both information as [name] => [pos]
  std::map<std::string, unsigned int> sorted;
  unsigned int pos = 1;
  for (auto name : names)
    sorted[name] = pos++;

#ifdef ORIGINAL
  // original problem
  unsigned int sum = 0;
  for (auto name : sorted)
  {
    unsigned int value = 0;
    // 'A' = 1, 'B' = 2, ..., 'Z' = 26
    for (auto c : name.first)
      value += c - 'A' + 1;
    // multiply by position
    sum += value * name.second;
  }
  std::cout << sum << std::endl;

#else

  unsigned int queries;
  std::cin >> queries;
  while (queries--)
  {
    std::string name;
    std::cin >> name;

    unsigned int value = 0;
    // 'A' = 1, 'B' = 2, ..., 'Z' = 26
    for (auto c : name)
      value += c - 'A' + 1;
    // multiply by position
    value *= sorted[name];

    std::cout << value << std::endl;
  }
#endif

  return 0;
}
