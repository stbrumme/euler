// ////////////////////////////////////////////////////////
// # Title
// Crack-free Walls
//
// # URL
// https://projecteuler.net/problem=215
// http://euler.stephan-brumme.com/215/
//
// # Problem
// Consider the problem of building a wall out of 2x1 and 3x1 bricks (horizontal x vertical dimensions) such that,
// for extra strength, the gaps between horizontally-adjacent bricks never line up in consecutive layers, i.e. never form a "running crack".
//
// For example, the following 9x3 wall is not acceptable due to the running crack shown in red:
//
// ![wall](https://projecteuler.net/project/images/p215_crackfree.gif)
//
// There are eight ways of forming a crack-free 9x3 wall, written W(9,3) = 8.
//
// Calculate W(32,10).
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// My solution can be divided in 3 steps:
// 1. ''generateRows'': find every possible sequence of "long" and "short" bricks, store them in ''allRows''
// 2. ''checkCompatibility'': compare each possible row to each other to figure out which rows can be placed next to each other without running cracks, store in ''compatible''
// 3. ''count'': determine how many rows can be below the current row, based on information in ''compatible''
//
// My rows contain the positions of the edges of two bricks (the "cracks"). The first and last are omitted because they are always 0 and 32 and not considered to be a crack.
// The first row of the 9x3 wall would be ''{ 3, 5, 7 }'', the second ''{ 2, 4, 7 }'' and the bottom ''{ 3, 6 }''.
//
// Step 1 was implemented as a recursive function that adds a "long" and a "short" brick to a row until its width exceeds 32.
// Step 2 compares all rows and contains a stripped-down version of ''std::intersection'' to find values contained in two containers.
// Step 3 requires a bit of memoization but is the most simple step: it calls itself with all compatible rows.

#include <iostream>
#include <vector>

// a single row of brick, store the distance of each crack from the left border
typedef std::vector<unsigned char> Row;
// all unique rows
std::vector<Row> allRows;

// for each row of allRows store the indices of potential neighboring rows
std::vector<std::vector<unsigned int>> compatible;

// generate all distinct row patterns and store them in allRows
void generateRows(Row row, unsigned char maxWidth)
{
  // last element contains the width of current row
  auto width = row.empty() ? 0 : row.back();

  // just one brick left ?
  if (width + 2 == maxWidth || width + 3 == maxWidth)
  {
    allRows.push_back(row);
    return;
  }
  // gap is too small for a brick
  if (width + 1 == maxWidth)
    return;

  // add a 2-brick
  row.push_back(width + 2);
  generateRows(row, maxWidth);

  // add a 3-brick
  row.back()++; // replace last 2-brick by a 3-brick
  generateRows(row, maxWidth);
}

// find all rows that can be neighbors
void checkCompatibility()
{
  compatible.resize(allRows.size());

  for (size_t i = 0; i < allRows.size(); i++)
    for (size_t j = i + 1; j < allRows.size(); j++)
    {
      // verify that both rows share no crack
      bool valid = true;

      // similar to implementation of std::intersection
      // note: all rows are already sorted
      auto current1 = allRows[i].begin();
      auto current2 = allRows[j].begin();
      while (current1 != allRows[i].end() && current2 != allRows[j].end())
      {
        if (*current1 < *current2)
          current1++;
        else
          if (*current2 < *current1)
            current2++;
        else
        {
          valid = false;
          break;
        }
      }

      // yes, both rows are compatible to each other
      if (valid)
      {
        compatible[i].push_back(j);
        compatible[j].push_back(i);
      }
    }
}

// count crack-free walls
unsigned long long count(unsigned int rowId, unsigned int rowsLeft)
{
  // last row ?
  if (rowsLeft == 1)
    return 1;

  static std::vector<std::vector<unsigned long long>> cache(allRows.size());
  const unsigned long long Invalid = 0;
  // try to look up memoized result
  if (cache[rowId].size() <= rowsLeft)
    cache[rowId].resize(rowsLeft + 1, Invalid); // not known yet, allocate memory
  else
    if (cache[rowId][rowsLeft] != Invalid)
      return cache[rowId][rowsLeft];

  // process all compatible walls
  unsigned long long result = 0;
  for (auto x : compatible[rowId])
    result += count(x, rowsLeft - 1);

  cache[rowId][rowsLeft] = result;
  return result;
}

int main()
{
  unsigned int width  = 32;// 9;
  unsigned int height = 10;// 3;
  std::cin >> width >> height;

  // create all 3329 distinct rows
  Row empty;
  generateRows(empty, width);

  // set up compatibility relationships
  checkCompatibility();

  // start with each known row and count generated walls
  unsigned long long result = 0;
  for (unsigned int i = 0; i < allRows.size(); i++)
    result += count(i, height);

  std::cout << result << std::endl;
  return 0;
}
