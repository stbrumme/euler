// ////////////////////////////////////////////////////////
// # Title
// Tri-colouring a triangular grid
//
// # URL
// https://projecteuler.net/problem=189
// http://euler.stephan-brumme.com/189/
//
// # Problem
// Consider the following configuration of 64 triangles:
//
// ![grid](p189_grid.gif)
// We wish to colour the interior of each triangle with one of three colours: red, green or blue, so that no two neighbouring triangles have the same colour. Such a colouring shall be called valid. Here, two triangles are said to be neighbouring if they share an edge.
// Note: if they only share a vertex, then they are not neighbours.
//
// For example, here is a valid colouring of the above grid:
//
// ![colored grid](p189_colours.gif)
// A colouring C' which is obtained from a colouring C by rotation or reflection is considered distinct from C unless the two are identical.
//
// How many distinct valid colourings are there for the above configuration?
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// I solved this problem in classic Dynamic Programming way:
// - generate each possible row triangle-by-triangle, beginning with the top row
// - whenever I start a new row check if I already fully processed a row __similar__ to the most previous row
//
// The trick is to define the word "similar": I call rows similar to each other if they are followed by the same number of combinations for all rows beneath it.
// For example, each color of the top-most triangle is similar to each other because I can permute the color such that I can produce the original coloring.
//
// My function ''getId()'' returns a hash value of a row stored in ''triangle''. If two rows share the same hash then they must be similar.
// Only a row's triangles that share an edge with the row beneath it are relevant (in general: each triangle with an even index).
// In the third row of the example, only the left-most green and the two blue triangles have to be considered by ''getId()''.
//
// Each color is represented by an integer: Red = 1, Green = 2, Blue = 3.
// Initially I used these codes to get a hash value
// `id = row * (color[0] * 3^0 + color[2] * 3^1 + color[4] * 3^2 * ...)`
//
// Then it occurred to me that the differences between those relevant triangles are sufficient (plus 3 if difference is negative):
// `id = row * ((color[2] - color[0]) * 3^0 + (color[4] - color[2]) * 3^1 + (color[6] - color[4]) * 3^2 + ...)`
//
// Due to symmetry, each row is similar to itself when all relevant triangles are in reverse order (see ''#define SYMMETRIC_ID'').
// So I compute two hashes and choose the smaller one because I want to have as many rows as possible to share the same hash value / ID.
//
// # Hackerrank
// The number of colors can be larger than 3 and the maximum height of the triangle is 11.
// My solution times out for about every fifth test case.
//
// # Alternative
// There is something called "chromatic polynomials" which can solve this problem, too (see https://en.wikipedia.org/wiki/Chromatic_polynomial ).
// I haven't tried it but it seems to be a faster way.

#include <iostream>
#include <vector>
#include <map>

#define ORIGINAL     // disable for Hackerrank
#define SYMMETRIC_ID // identify rows in reverse order as similar

// total height
unsigned int height    = 8;
unsigned int numColors = 3;

// "scratchpad", used while processing the colorings
typedef char Color;
std::vector<Color> triangles;

// cache result for completed rows, index is determined by getId()
#ifdef ORIGINAL
std::vector<unsigned long long> cache(5102 + 1, 0); // sufficient for height<=8 and numColors<=3
#else
std::map<unsigned int, unsigned long long> cache;   // handle bigger input values, too
#endif

// compute hash value of a row
unsigned int getId(unsigned int row)
{
  auto first = row * row;
  auto width = 2 * row + 1;

  // compute hash of triangles: find differences between adjacent triangles of the same orientation
  unsigned int result = row;
  for (auto i = first + 2; i < first + width; i += 2)
  {
    auto diff = triangles[i - 2] - triangles[i];
    if (diff < 0)
      diff += numColors;

    result = result * numColors + diff;
  }

#ifdef SYMMETRIC_ID
  // simple optimization based on symmetry: process the same triangles in reverse order:
  unsigned int reverse = row;
  for (auto i = first + width - 1; i >= first + 2; i -= 2)
  {
    // exactly the same as above but "i" is running in reverse
    auto diff = triangles[i - 2] - triangles[i];
    if (diff < 0)
      diff += numColors;

    reverse = reverse * numColors + diff;
  }
  if (result > reverse)
    result = reverse;
#endif

  return result;
}

// simple enumeration, solves triangle with 5 rows in about a second
unsigned long long search(unsigned int row = 0, unsigned int column = 0)
{
  // index of the first triangle in the current row
  auto first = row * row;
  // index of the current triangle
  auto index = first + column;
  // number of triangles in the current row
  auto width = 2 * row + 1;

  // "coordinates" of the next triangle
  auto nextRow    = row;
  auto nextColumn = column + 1;
  if (nextColumn == width)
  {
    // start a new row
    nextRow++;
    nextColumn = 0;
  }

  // hash value of the previous row, only needed when starting a new row
  unsigned int prevId = 0;
  if (column == 0)
  {
    // finished ?
    if (row == height)
      return 1; // found another valid coloring

    if (row == 0)
      cache.clear(); // only relevant when computing multiple triangle sizes (as I did while developing my solution)
    else // row > 0
    {
      // already cached ?
      prevId = getId(row - 1);
      if (cache[prevId] != 0)
        return cache[prevId];
    }
  }

  // recursively process current position
  unsigned long long result = 0;
  if (column % 2 == 0) // even: no contact to triangle in row above
    for (Color color = 1; color <= (Color)numColors; color++)
    {
      // exclude color of left neighbor (but first triangle in a row can have any color because it has no left neighbor)
      if (column > 0 && triangles[index - 1] == color)
        continue;
      triangles[index] = color;
      result += search(nextRow, nextColumn);
    }
  else                 // odd: must respect triangle in row above
    for (Color color = 1; color <= (Color)numColors; color++)
    {
      // exclude color of left neighbor and neighbor in row above
      if (triangles[index - 1] == color || triangles[index - 2 * row] == color)
        continue;
      triangles[index] = color;
      result += search(nextRow, nextColumn);
    }

#ifndef ORIGINAL
  // Hackerrank only
  result %= 1000000007;
#endif

  // store result
  if (column == 0 && row > 0)
    cache[prevId] = result;

  return result;
}

int main()
{
  std::cin >> height >> numColors;
  triangles.resize(height * height);

  // let's go !
  std::cout << search() << std::endl;
  return 0;
}
