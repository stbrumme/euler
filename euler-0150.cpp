// ////////////////////////////////////////////////////////
// # Title
// Searching a triangular array for a sub-triangle having minimum-sum
//
// # URL
// https://projecteuler.net/problem=150
// http://euler.stephan-brumme.com/150/
//
// # Problem
// In a triangular array of positive and negative integers, we wish to find a sub-triangle such that the sum of the numbers it contains is the smallest possible.
//
// In the example below, it can be easily verified that the marked triangle satisfies this condition having a sum of −42.
//
// ![triangle](p150.gif)
//
// We wish to make such a triangular array with one thousand rows, so we generate 500500 pseudo-random numbers `s_k` in the range `\pm 2^19`,
// using a type of random number generator (known as a Linear Congruential Generator) as follows:
//
// ''t := 0''
// ''for k = 1 up to k = 500500:''
// ''  t := (615949*t + 797807) modulo 220''
// ''  `s_k` := t-219''
//
// Thus: `s_1 = 273519`, `s_2 = -153582`, `s_3 = 450905` etc
//
// Our triangular array is then formed using the pseudo-random numbers thus:
// ||   2   ||   2   ||   2   ||   2   ||   2   ||   2   ||    2   ||
// ||       ||       ||       || `s_1` ||       ||       ||        ||
// ||       ||       || `s_2` ||       || `s_3` ||       ||        ||
// ||       || `s_4` ||       || `s_5` ||       || `s_6` ||        ||
// || `s_7` ||       || `s_8` ||       || `s_9` ||       || `s_10` ||
// ...
//
// Sub-triangles can start at any element of the array and extend down as far as we like
// (taking-in the two elements directly below it from the next row, the three elements directly below from the row after that, and so on).
//
// The "sum of a sub-triangle" is defined as the sum of all the elements it contains.
// Find the smallest possible sub-triangle sum.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// My solution can be called a "tweaked brute-force approach".
// I store the triangle in a 2D array, where the number of "columns" of each row varies: only one at the top and columns = rows at the bottom.
// For each cell (x,y) I walk through all possible triangles (x, y),(x + increment, y),(x, y + increment).
// For increment = 0, the sum is just the value found in ''triangle[x][y]''.
// For increment = 1, I add all the two values of the next row to the result of increment = 0, thus ''triangle[x][y] + triangle[x][y+1] + triangle[x+1][y+1]''
// In general: for each increment > 0, I add all the two values of the next row to the result of increment - 1.
//
// Whenever my sum is lower than the previously lowest sum, I store it (Hackerrank: store every sum).
//
// Repeatedly adding numbers in the same row turned out to be a performance bottleneck.
// In step 2 of my code, a second container called ''sums'' is created which contains ''sums[x][y] = sum(triangle[0..x][y])'' (similar to a running total).
// Then ''sum(triangle[x1..x2][y]) = sums[x2][y] - sums[x1 - 1][y]'' (except when x1 is zero).
// In the end, this optimization reduced runtime from 20 seconds to just 0.4 seconds.
//
// # Hackerrank
// Hackerrank provides all input data (instead of the Linear Congruential Generator) and asks you to print
// the n smallest sub-triangle sums.
//
// Because of the large number of sums (= high memory usage), I repeatedly sort all values of ''results'' and erase all but the smallest n.
//
// I am fully aware that the high number if ''#ifdef'' severely decreases readability of my code.

#include <iostream>
#include <vector>
#include <algorithm>

// pseudo-random number generator
int lcg()
{
  static long long seed = 0;
  seed = (615949 * seed + 797807) % (1 << 20);
  return int(seed - (1 << 19));
}

int main()
{
  // indexed: triangle[y][x] where y denotes the row
  std::vector<std::vector<int>> triangle;

  // step 1: initialize the triangle (either with random data or from STDIN)
#define ORIGINAL
#ifdef ORIGINAL
  // 1000 rows
  unsigned int maxSize = 1000;
  std::cin >> maxSize;

  // fill with (pseudo) random data
  triangle.resize(maxSize);
  for (size_t y = 0; y < triangle.size(); y++)
    for (size_t x = 0; x <= y; x++)
      triangle[y].push_back(lcg());

#else
  size_t size;
  size_t maxResults;
  std::cin >> size >> maxResults;

  triangle.resize(size);
  for (size_t y = 0; y < size; y++)
    for (size_t x = 0; x <= y; x++)
    {
      int current;
      std::cin >> current;
      triangle[y].push_back(current);
    }
#endif

  // step 2: build another triangle called "sums" where each cell is sum(triangle[y][0..x])
  std::vector<std::vector<int>> sums(triangle.size());
  for (size_t y = 0; y < triangle.size(); y++)
  {
    // same size
    sums[y].resize(triangle[y].size());

    // store running sum of all cells to the left of the current cell, including the current, too
    int rowSum = 0;
    for (size_t x = 0; x <= y; x++)
    {
      rowSum += triangle[y][x];
      sums[y][x] = rowSum;
    }
  }

  // store the smallest sums
#ifdef ORIGINAL
  auto result = triangle[0][0]; // simplify if only one sum
#else
  std::vector<int> results;   // needs sorting, pruning, etc.
#endif

  // analyze all positions
  for (size_t y = 0; y < triangle.size(); y++)
  {
    for (size_t x = 0; x <= y; x++)
    {
      // smallest triangle consists of a single-cell
      auto sum = triangle[y][x];

      // better than before ?
#ifdef ORIGINAL
      if (result > sum)
        result = sum;
#else
      results.push_back(sum);
#endif

      // add rows
      auto maxSize = triangle.size() - y;
      for (size_t current = 1; current < maxSize; current++)
      {
        // compute sum(triangle[y + rows][x ... x + rows])
        // add singles values, "old way"
        //for (size_t scan = x; scan <= x + current; scan++)
        //  sum += triangle[y + current][scan];

        // access pre-computed running sums from step 2, "new way"
        auto rowSum = sums[y + current][x + current];
        if (x > 0)
          rowSum -= sums[y + current][x - 1];
        sum += rowSum;

        // better than before ?
#ifdef ORIGINAL
        if (result > sum)
          result = sum;
#else
        results.push_back(sum);
#endif
      }
    }

#ifndef ORIGINAL
    // prune sums
    std::sort(results.begin(), results.end());
    if (results.size() > maxResults)
      results.erase(results.begin() + maxResults, results.end());
#endif
  }

  // show result
#ifdef ORIGINAL
  std::cout << result << std::endl;
#else
  for (auto x : results)
    std::cout << x << std::endl;
#endif

  return 0;
}
