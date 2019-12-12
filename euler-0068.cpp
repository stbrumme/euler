// ////////////////////////////////////////////////////////
// # Title
// Magic 5-gon ring
//
// # URL
// https://projecteuler.net/problem=68
// http://euler.stephan-brumme.com/68/
//
// # Problem
// Consider the following "magic" 3-gon ring, filled with the numbers 1 to 6, and each line adding to nine.
// ![magic 3-gon](p068_1.gif)
// Working clockwise, and starting from the group of three with the numerically lowest external node (4,3,2 in this example),
// each solution can be described uniquely. For example, the above solution can be described by the set: 4,3,2; 6,2,1; 5,1,3.
//
// It is possible to complete the ring with four different totals: 9, 10, 11, and 12. There are eight solutions in total.
// ||   4   ||      10             ||
// || Total || Solution Set        ||
// ||   9   || 4,2,3; 5,3,1; 6,1,2 ||
// ||   9   || 4,3,2; 6,2,1; 5,1,3 ||
// ||  10   || 2,3,5; 4,5,1; 6,1,3 ||
// ||  10   || 2,5,3; 6,3,1; 4,1,5 ||
// ||  11   || 1,4,6; 3,6,2; 5,2,4 ||
// ||  11   || 1,6,4; 5,4,2; 3,2,6 ||
// ||  12   || 1,5,6; 2,6,4; 3,4,5 ||
// ||  12   || 1,6,5; 3,5,4; 2,4,6 ||
//
// By concatenating each group it is possible to form 9-digit strings; the maximum string for a 3-gon ring is 432621513.
//
// Using the numbers 1 to 10, and depending on arrangements, it is possible to form 16- and 17-digit strings. What is the maximum 16-digit string for a "magic" 5-gon ring?
// ![magic 5-gon](p068_2.gif)
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// I split the graph into two parts:
// - an "inner ring"
// - and an "outer ring"
// In the initial example, the inner ring consists of 1,2,3 while the outer ring contains 4,5,6.
//
// Each cell of the inner ring is part of two lines. It is indexed from ''0'' to ''size - 1'' (''size'' is 3 for a 3-gon, 5 for a 5-gon, etc.).
// Each cell of the outer ring is part of only one line. It is indexed from ''size'' to ''2*size - 1''.
//
// The idea is to fill the inner ring using backtracking.
// Whenever two out of three cells of a line are known, the third can be directly computed.
// If that third number is already used, then we have to backtrack.
//
// My function ''fillLine'' fills the inner ring: it tries all combination of available numbers for the cell at index ''pos''.
// Whenever it succeeds, it calls itself (recursion) to fill ''pos + 1''.
//
// All available numbers are stored in a bitmask called ''used''. Number ''x'' is available if ''(used & (1 << x)) == 0''.
// At the end, ''result'' contains all valid n-gons.
//
// Project Euler asks for the maximum string while Hackerrank asks for all strings in ascending order.
// My solution prints the Hackerrank result and I manually have to choose the larger one (there are just two solutions).
//
// The correct result will be printed when ''size = 5'' and ''tripletSum = 14'', that means the program input is ''"5 14"''.
// There are solutions for ''"5 16", "5 17" and "5 19"'' but they are either lexicographically smaller or have 17 instead of 16 digits.

#include <iostream>
#include <vector>
#include <string>
#include <set>

// sorted container of valid n-gon strings
std::set<std::string> result;
// size of the n-gon (3,4,5)
unsigned int size;
// sum of all three cells alongs a diagonal/edge
unsigned int tripletSum;

void fillLine(unsigned int pos, std::vector<unsigned int> inner, std::vector<unsigned int> outer, unsigned int used)
{
  // inner ring completely filled, just one cell of the outer ring left
  if (pos == size - 1)
  {
    // check last line
    outer[size - 1] = tripletSum - (inner[0] + inner[size - 1]);
    unsigned int mask = 1 << outer[size - 1];
    if ((used & mask) != 0)
      return;

    // first element of outer ring must be the smallest
    for (auto x : outer)
      if (x < outer[0])
        return;

    // build string
    std::string id;
    for (unsigned int i = 0; i < size; i++)
      id += std::to_string(outer[i]) + std::to_string(inner[i]) + std::to_string(inner[(i + 1) % size]);

    // will be alphabetically ordered
    result.insert(id);
    return;
  }

  // move a number between 1 and 2*size into one of the inner cells of the n-gon
  for (unsigned int i = 1; i <= 2*size; i++)
  {
    // fill a cell of the inner ring
    unsigned int innerMask = 1 << i;
    // is that number still available ?
    if ((innerMask & used) != 0)
      continue;

    // occupy cell
    inner[pos + 1] = i;
    unsigned int nextUsed = used | innerMask;

    // compute the according cell in the outer ring
    outer[pos] = tripletSum - (inner[pos] + i);
    unsigned int outerMask = 1 << outer[pos];
    // is that number still available ?
    if ((nextUsed & outerMask) != 0)
      continue;
    nextUsed |= outerMask;

    // next line
    fillLine(pos + 1, inner, outer, nextUsed);
  }
}

int main()
{
  std::cin >> size >> tripletSum;

  // generate the inner and outer ring
  std::vector<unsigned int> inner(size);
  std::vector<unsigned int> outer(size);
  // a triplet consists of inner[a], inner[(a+1) % (2*a)], outer[a]

  // generate a bitmask of allowed numbers (0 = still available, 1 = already used / disallowed)
  unsigned int allowed = 0;
  for (unsigned int i = 1; i <= 2 * size; i++)
    allowed |= 1 << i;
  allowed = ~allowed;

  // fill first cell of inner ring
  for (unsigned int i = 1; i <= 2*size; i++)
  {
    inner[0] = i;
    // fill remaining cells
    fillLine(0, inner, outer, allowed | (1 << i));
  }

//#define ORIGINAL
#ifdef ORIGINAL
  std::cout << *result.rbegin() << std::endl;
#else
  for (auto r : result)
    std::cout << r << std::endl;
#endif
}
