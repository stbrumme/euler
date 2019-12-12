// ////////////////////////////////////////////////////////
// # Title
// Gnomon numbering
//
// # URL
// https://projecteuler.net/problem=412
// http://euler.stephan-brumme.com/412/
//
// # Problem
// For integers `m`, `n` (`0 <= n < m`), let `L(m, n)` be an `m * m` grid with the top-right `n * n` grid removed.
//
// For example, `L(5, 3)` looks like this:
//
// ![L shape](p412_table53.png)
//
// We want to number each cell of `L(m, n)` with consecutive integers 1, 2, 3, ... such that the number in every cell is smaller than the number below it and to the left of it.
//
// For example, here are two valid numberings of `L(5, 3)`:
//
// ![examples](p412_tablenums.png)
//
// Let `LC(m, n)` be the number of valid numberings of `L(m, n)`.
// It can be verified that `LC(3, 0) = 42`, `LC(5, 3) = 250250`, `LC(6, 3) = 406029023400` and `LC(10, 5) mod 76543217 = 61251715`.
//
// Find `LC(10000, 5000) mod 76543217`.
//
// # Written by
// Stephan Brumme
// December 2017
//
// # Idea
// Okay, at the moment I have no idea how to solve that problem.
//
// But it was fun spending a few minutes on writing a brute-force solution for grids no larger than 6x6.
// By the way: ''bruteForce(6,3)'' finished after about 7.5 hours ...
// The next morning I wrote ''slow'' which is based on the following observation:
// - when filling the grid "in reverse order" (that means from 16 to 1), then smaller numbers are automatically stacked on larger numbers
// - each column of the grid can be seen as a "skyscraper"
// - there are no gaps in a skyscraper
// - each skyscraper is at most as high as its left neighbor
// In the end I don't have to store the single numbers in the grid (a 2D data structure).
// Instead I just need to track the height of each skyscraper (a 1D data structure).
// These heights are identical for many different number distributions so that I can accelerate the search by aggressive memoization.
// ''slow(6,3)'' finds the correct result in less than 0.01 seconds (vs. 7.5 hours !).
// Note: all results are displayed mod 76543217.
//
// However, this improved algorithm can't solve `LC(10000, 5000)` in a reasonable timeframe.

#include <iostream>
#include <vector>
#include <map>

const auto MaxSize = 10; // no grids larger than 10x10 (6x6 is already super-slow in brute-force mode)
auto size   = 5;        // that's m from the problem statement
auto cutout = 3;        // that's n from the problem statement

const auto Modulo  = 76543217;

// indicates a cell that hasn't be filled yet
const unsigned int Unknown = 0;
unsigned int cells[MaxSize][MaxSize] = { Unknown };

// add "number" and all smaller numbers to "cells", return how many combinations where found
unsigned long long bruteForce(unsigned int number)
{
  // successfully filled all cells ?
  if (number == 0)
  {
    static unsigned long long progress = 0;
    if (++progress % (1 << 24) == 0)
      std::cout << (progress * 100.0 / 406029023400) << "%" << std::endl;
    return 1;
  }

  unsigned long long result = 0;

  // iterate over all free spots where the left and lower neighbor are bigger (or there is no neighbor => border)
  for (auto y = 0; y < size; y++)
  {
    // avoid the cut-out area
    auto maxX = size;
    if (y < cutout)
      maxX = size - cutout;

    for (auto x = 0; x < maxX; x++)
    {
      // cell previously filled ?
      if (cells[x][y] != Unknown)
        continue;

      // all bigger numbers were already processed, so there's no gap allowed on the left or bottom

      // left   neighbor must exist, or there's the grid's border
      if (x > 0 && cells[x-1][y] == Unknown)
        continue;
      // bottom neighbor must exist, or there's the grid's border
      if (y < size - 1 && cells[x][y+1] == Unknown)
        continue;

      // okay, found an empty cell !
      cells[x][y] = number;
      // go deeper
      result += bruteForce(number - 1);

      // undo current step and keep looking for other configurations
      cells[x][y] = Unknown;
    }
  }

  return result % Modulo;
}

// my "skyscraper" algorithm
typedef std::vector<unsigned short> HeightMap;
unsigned long long slow(unsigned int number, HeightMap& height)
{
  // successfully filled all cells ?
  if (number == 1)
    return 1;

  // memoize
  static std::map<HeightMap, unsigned long long> cache;
  auto lookup = cache.find(height);
  if (lookup != cache.end())
    return lookup->second;

  // find a column that isn't full and is lower than its left neighbor
  unsigned long long result = 0;
  for (auto i = 0; i < size; i++)
  {
    // maximum height
    auto maxHeight = size;
    if (i >= size - cutout)
      maxHeight -= cutout;

    if (height[i] < maxHeight)
      // lower than its left neighbor ?
      if (i == 0 || height[i] < height[i - 1])
      {
        height[i]++;
        result += slow(number - 1, height);
        height[i]--;
      }
  }

  result %= Modulo;
  cache[height] = result;
  return result;
}

int main()
{
  std::cin >> size >> cutout;
  // for live tests only
  if (size > MaxSize || size < cutout)
    return 1;

  // the highest number is always located in the lower-right corner
  auto highestNumber = size * size - cutout * cutout;

  // brute force
  //std::cout << bruteForce(highestNumber) << std::endl;

  // significantly faster, but still too slow to solve the problem (it easily solves all examples)
  HeightMap height(size, 0);
  std::cout << slow(highestNumber, height) << std::endl;

  return 0;
}
