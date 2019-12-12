// ////////////////////////////////////////////////////////
// # Title
// Triominoes
//
// # URL
// https://projecteuler.net/problem=161
// http://euler.stephan-brumme.com/161/
//
// # Problem
// A triomino is a shape consisting of three squares joined via the edges. There are two basic forms:
//
// ![basic forms](p161_trio1.gif)
//
// If all possible orientations are taken into account there are six:
//
// ![all forms](p161_trio3.gif)
//
// Any n by m grid for which nxm is divisible by 3 can be tiled with triominoes.
// If we consider tilings that can be obtained by reflection or rotation from another tiling as different
// there are 41 ways a 2 by 9 grid can be tiled with triominoes:
//
// ![grid 9x2](p161_k9.gif)
//
// In how many ways can a 9 by 12 grid be tiled in this way by triominoes?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I wrote a typical Dynamic Programming solution:
// - the grid is filled from top to bottom
// - bit masks represent whether a cell of the grid is occupied (=1) or available/empty (=0)
// - ''search'' attempts to place every shape in the top-most and left-most empty cell
//
// There are a few observations:
// 1. All rows are either empty, partially filled or full
// 2. There are at most 3 partially filled rows (due to my strict "from-top-to-bottom" algorithm)
//
// My memoization hash therefore consists only of the number of empty rows and the bitmasks of (up to) three partially filled rows.
// For the 9x12 grid, it just fits into a 32 bit integer.
//
// The number of solutions for a grid of size n x m is identical to a grid of size m x n.
// The hash is "smaller" and creates more opportunities for memoization hits when the rows are small.
//
// # Hackerrank
// The result has to be printed modulo ''1000000007''.
//
// # Note
// I was surprised to find only 223255 values in ''cache'' (for the 9x12 grid).
// On the other side, the rotated 12x9 grid would cause over 5 million values (and run about 70x slower).
// However, my ''std::swap'' optimization automatically converts the 12x9 grid to a 9x12 grid.

#include <iostream>
#include <unordered_map>

#define ORIGINAL

// grid size
unsigned int height = 9;
unsigned int width  = 2;

// indicate an empty row
const unsigned int EmptyRow = 0;

// set a certain bit to one (= position is not available anymore), return true if bit was zero
// note: second parameter is an in/out parameter
bool use(unsigned int pos, unsigned int& row)
{
  unsigned int mask = 1 << pos;
  bool result = (row & mask) == 0;
  row |= mask;
  return result;
}

// recursive search
unsigned long long search(unsigned int rowsLeft,
                          unsigned int rowA, unsigned int rowB, unsigned int rowC)
{
  // done ?
  if (rowsLeft == 0)
    return 1;

  // filled another row ?
  unsigned int fullRow = (1 << width) - 1;
  if (rowA == fullRow)
    return search(rowsLeft - 1, rowB, rowC, EmptyRow);

  // find first gap in rowA
  unsigned int pos = 0;
  while ((rowA & (1 << pos)) != 0)
    pos++;

  // unique ID
  unsigned long long hash; // unsigned int would suffice for grid with up to 9 columns, too
  hash   = rowsLeft;
  hash <<= width;
  hash  |= rowA;
  hash <<= width;
  hash  |= rowB;
  hash <<= width;
  hash  |= rowC;

  // about twice as fast as std::map
  static std::unordered_map<unsigned long long, unsigned long long> cache;
  auto i = cache.find(hash);
  if (i != cache.end())
    return i->second;

  unsigned long long result = 0;

  // shape: ##
  //        #
  unsigned int a = rowA;
  unsigned int b = rowB;
  unsigned int c = rowC;
  if (rowsLeft >= 2 && pos < width - 1 &&
      use(pos, a) && use(pos + 1, a) && use(pos, b))
    result += search(rowsLeft, a, b, c);

  // shape: ##
  //         #
  a = rowA; b = rowB; //c = rowC;
  if (rowsLeft >= 2 && pos < width - 1 &&
      use(pos, a) && use(pos + 1, a) && use(pos + 1, b))
    result += search(rowsLeft, a, b, c);

  // shape: #
  //        ##
  a = rowA; b = rowB; //c = rowC;
  if (rowsLeft >= 2 && pos < width - 1 &&
      use(pos, a) && use(pos, b) && use(pos + 1, b))
    result += search(rowsLeft, a, b, c);

  // shape:  #
  //        ##
  // note: this shape extends one "negative" unit to the left
  a = rowA; b = rowB; //c = rowC;
  if (rowsLeft >= 2 && pos > 0 && pos < width &&
      use(pos, a) && use(pos - 1, b) && use(pos, b))
    result += search(rowsLeft, a, b, c);

  // shape:  #
  //         #
  //         #
  a = rowA; b = rowB; //c = rowC;
  if (rowsLeft >= 3 && pos < width &&
      use(pos, a) && use(pos, b) && use(pos, c))
    result += search(rowsLeft, a, b, c);

  // shape: ###
  a = rowA; b = rowB; c = rowC;
  if (rowsLeft >= 1 && pos < width - 2 &&
      use(pos, a) && use(pos + 1, a) && use(pos + 2, a))
    result += search(rowsLeft, a, b, c);

#ifndef ORIGINAL
  result %= 1000000007; // Hackerrank only
#endif

  // memoize
  cache[hash] = result;
  return result;
}

int main()
{
  // read grid size
  std::cin >> width >> height;

  // prefer fewer columns
  if (width > height)
    std::swap(width, height);

  // let's go !
  std::cout << search(height, EmptyRow, EmptyRow, EmptyRow) << std::endl;
  return 0;
}
