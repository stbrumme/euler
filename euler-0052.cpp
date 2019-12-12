// ////////////////////////////////////////////////////////
// # Title
// Permuted multiples
//
// # URL
// https://projecteuler.net/problem=52
// http://euler.stephan-brumme.com/52/
//
// # Problem
// It can be seen that the number, 125874, and its double, 251748, contain exactly the same digits, but in a different order.
// Find the smallest positive integer, x, such that 2x, 3x, 4x, 5x, and 6x, contain the same digits.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My function ''fingerprint'' counts how often each digit occurs and produces an integer (which may have up to 10 digits).
// The n-th decimal digit of the result represents how often the digit n occurs in the input, e.g.
// ''fingerprint(454430) = 131001''
// because ''5'' appears once, ''4'' three times, ''3'' once, no ''2'', no ''1'' and a single zero.
// ''fingerprint'' has the nice property that two number with the same fingerprint are a permutation of each other
// (phrased in the words of the problem statement: "contain the same digits").
//
// __Note:__ my fingerprint technique allows only up 9 identical digits which is okay because ''x'' has at most seven digits.
//
// I compute the fingerprint of each number ''i'', beginning with 1, and multiply it by 2, 3, 4, ...
// If the product still has the same fingerprint, then it is a permutation.
//
// # Note
// The is plenty of room for optimization. For example, if ''maxMultiple >= 5'' then the first digit of ''i'' must be a ''1''.
//
// # Hackerrank
// The number of multiples can be adjusted from 2 to 6 (the latter being the default value for the original problem).

#include <iostream>

// I generate a "fingerprint" for each number:
// e.g. a fingerprint of 40231 means that the parameter had
// 1 zero
// 3 ones
// 2 threes
// no fours
// 4 fives
// and no sixes, sevens, ...
unsigned long long fingerprint(unsigned int x)
{
  unsigned long long result = 0;

  while (x > 0)
  {
    // extract right-most digit
    auto digit = x % 10;
    x /= 10;

    // add 10^digit
    unsigned long long pos = 1;
    for (unsigned int i = 1; i <= digit; i++)
      pos *= 10;
    result += pos;
  }

  return result;
}

int main()
{
  // the result can be found with 1000000 6
  unsigned int maxNumber   = 1000000;
  unsigned int maxMultiple = 6;;
  std::cin >> maxNumber >> maxMultiple;

  // look at all numbers
  for (unsigned int i = 1; i <= maxNumber; i++)
  {
    // initial fingerprint
    auto id = fingerprint(i);

    bool found = true;
    for (unsigned int multiple = 2; multiple <= maxMultiple; multiple++)
      // mismatch ? => abort
      if (id != fingerprint(i * multiple))
      {
        found = false;
        break;
      }

    // print result
    if (found)
    {
//#define ORIGINAL
#ifdef ORIGINAL
      std::cout << i << std::endl;
      return 0;
#endif

      for (unsigned int multiple = 1; multiple <= maxMultiple; multiple++)
        std::cout << (i * multiple) << " ";
      std::cout << std::endl;
    }
  }

  return 0;
}
