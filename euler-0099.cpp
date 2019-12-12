// ////////////////////////////////////////////////////////
// # Title
// Largest exponential
//
// # URL
// https://projecteuler.net/problem=99
// http://euler.stephan-brumme.com/99/
//
// # Problem
// Comparing two numbers written in index form like `2^11` and `3^7` is not difficult, as any calculator would confirm that `2^11 = 2048 < 3^7 = 2187`.
//
// However, confirming that `632382^518061 > 519432^525806` would be much more difficult, as both numbers contain over three million digits.
//
// Using [base_exp.txt](https://projecteuler.net/project/resources/p099_base_exp.txt) (right click and 'Save Link/Target As...'), a 22K text file containing one thousand lines with a base/exponent pair on each line,
// determine which line number has the greatest numerical value.
//
// NOTE: The first two lines in the file represent the numbers in the example given above.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// If `a^b < x^y` then `log{a^b} < log{x^y}` which means `b * log{a} < y * log{x}`.
// The logarithm fits easily in a ''double''.
//
// ''std::map'' is an ascendingly sorted container ==> its last element has the greatest numerical value.
//
// # Hackerrank
// Print the base and exponent of the k-sorted element.

#include <cmath>
#include <map>
#include <iostream>

#define ORIGINAL

int main()
{
#ifdef ORIGINAL
  // read all 1000 pairs, store [logarithm] => [index]
  std::map<double, unsigned int> data;
  for (unsigned int i = 1; i <= 1000; i++) // first line has index 1 (not 0)
  {
    unsigned int base, exponent;
    char comma; // skip commas in input file
    std::cin >> base >> comma >> exponent;

    // sort by exponent * log(base)
    data[exponent * log(base)] = i;
  }
  // return index of last input line
  std::cout << data.rbegin()->second << std::endl;

  return 0;

#else

  // how many pairs ?
  unsigned int numbers;
  std::cin >> numbers;

  // read all pairs, store [logarithm] => [base, exponent]
  std::map<double, std::pair<unsigned int, unsigned int>> data;
  for (unsigned int i = 1; i <= numbers; i++)
  {
    unsigned int base, exponent;
    std::cin >> base >> exponent;
    data[exponent * log(base)] = std::make_pair(base, exponent);
  }

  // which number of the sorted list should be printed ?
  unsigned int pos;
  std::cin >> pos;

  // std::map is sorted, jump to the position
  auto i = data.begin();
  std::advance(i, pos - 1); // input is 1-based

  // get result
  auto result   = i->second;
  auto base     = result.first;
  auto exponent = result.second;
  // and print it
  std::cout << base << " " << exponent << std::endl;

  return 0;
#endif
}
