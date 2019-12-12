// ////////////////////////////////////////////////////////
// # Title
// Maximum path sum I
//
// # URL
// https://projecteuler.net/problem=18
// http://euler.stephan-brumme.com/18/
//
// # Problem
// By starting at the top of the triangle below and moving to adjacent numbers on the row below, the maximum total from top to bottom is 23.
//
// ''   3''
// ''  7 4''
// '' 2 4 6''
// ''8 5 9 3''
//
// That is, 3 + 7 + 4 + 9 = 23.
//
// Find the maximum total from top to bottom of the triangle below:
//
// ''                     75''
// ''                    95 64''
// ''                  17 47 82''
// ''                 18 35 87 10''
// ''               20 04 82 47 65''
// ''              19 01 23 75 03 34''
// ''            88 02 77 73 07 63 67''
// ''           99 65 04 28 06 16 70 92''
// ''         41 41 26 56 83 40 80 70 33''
// ''        41 48 72 33 47 32 37 16 94 29''
// ''      53 71 44 65 25 43 91 52 97 51 14''
// ''     70 11 33 28 77 73 17 78 39 68 17 57''
// ''   91 71 52 38 17 14 91 43 58 50 27 29 48''
// ''  63 66 04 68 89 53 67 30 73 16 69 87 40 31''
// ''04 62 98 27 23 09 70 98 73 93 38 53 60 04 23''
//
// __NOTE:__ As there are only 16384 routes, it is possible to solve this problem by trying every route.
// However, Problem 67, is the same challenge with a triangle containing one-hundred rows; it cannot be solved by brute force, and requires a clever method! ;o)
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// The main idea is to build a data structure similar to the input data:
// but instead of just storing the raw input we store the biggest sum up to this point.
//
// All data is processed row-by-row
//
// Of course, the first row consists of a single number and it has no "parents", that means no rows above it.
// Therefore the "sum" is the number itself.
// This row now becomes my "parent row" called ''last''.
//
// For each element of the next rows I have to find its parents (some have one, some have two),
// figure out which parent is bigger and then add the current input to it.
// This sum is stored in ''current''.
//
// When a row is fully processed, ''current'' becomes ''last''.
// When all rows are processed, the largest element in ''last'' is the result of the algorithm.
//
// Example:
//
// ''  1''
// '' 2 3''
// ''4 5 6''
// initialize:
// ''last[0] = 1;''
//
// read second line:
// ''current[0] = 2 + last[0] = 3''
// ''current[1] = 3 + last[0] = 4''
// copy current to last (which becomes { 3, 4 })
//
// read third line:
// ''current[0] = 4 + last[0] = 7''
// ''current[1] = 5 + max(last[0], last[1]) = 9''
// ''current[2] = 6 + last[1] = 10''
// copy current to last (which becomes { 7, 9, 10 })
//
// finally:
// print max(last) = 10
//
// # Note
// Exactly the same algorithm is used for problem 67.

#include <iostream>
#include <vector>
#include <algorithm>

int main()
{
  unsigned int tests = 1;

//#define ORIGINAL
#ifndef ORIGINAL
  std::cin >> tests;
#endif

  while (tests--)
  {
    unsigned int numRows = 15;
#ifndef ORIGINAL
    std::cin >> numRows;
#endif

    // process input row-by-row
    // each time a number is read we add it to the two numbers above it
    // choose the bigger sum and store it
    // if all rows are finished, find the largest number in the last row

    // read first line, just one number
    std::vector<unsigned int> last(1);
    std::cin >> last[0];

    // read the remaining lines
    for (unsigned int row = 1; row < numRows; row++)
    {
      // prepare array for new row
      unsigned int numElements = row + 1;
      std::vector<unsigned int> current;

      // read all numbers of current row
      for (unsigned int column = 0; column < numElements; column++)
      {
        unsigned int x;
        std::cin >> x;

        // find sum of elements in row above (going a half step to the left)
        unsigned int leftParent = 0;
        // only if left  parent is available
        if (column > 0)
          leftParent = last[column - 1];

        // find sum of elements in row above (going a half step to the right)
        unsigned int rightParent = 0;
        // only if right parent is available
        if (column < last.size())
          rightParent = last[column];

        // add larger parent to current input
        unsigned int sum = x + std::max(leftParent, rightParent);
        // and store this sum
        current.push_back(sum);
      }

      // row is finished, it become the "parent" row
      last = current;
    }

    // find largest sum in final row
    std::cout << *std::max_element(last.begin(), last.end()) << std::endl;
  }

  return 0;
}
