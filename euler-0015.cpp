// ////////////////////////////////////////////////////////
// # Title
// Lattice paths
//
// # URL
// https://projecteuler.net/problem=15
// http://euler.stephan-brumme.com/15/
//
// # Problem
// Starting in the top left corner of a 2x2 grid, and only being able to move to the right and down,
// there are exactly 6 routes to the bottom right corner.
//
// ![Grid](p015.gif)
//
// How many such routes are there through a 20x20 grid?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Each grid consists of ''(width+1)*(height+1)'' points. The grid in the problem statement has ''(2+1)*(2+1)=9'' points.
// I chose my coordinates to be ''[0][0]'' in the top left corner and ''[width][height]'' in the bottom right corner.
//
// For each point in a grid, the number of routes from the current point to the lower right corner
// is the sum of all routes when going right plus all routes when going down:
// ''grid[x][y] = grid[x+1][y] + grid[x][y+1];''
//
// There is a single route from the lower right corner to itself ("stay where you are"):
// ''grid[width][height] = 1;''
//
// Now I perform a breadth-first search ( https://en.wikipedia.org/wiki/Breadth-first_search ) from the lower-right corner to the upper-left corner.
// A queue named ''next'' contains the coordinates of the next points to analyze - initially it holds the point above the lower-right corner
// and the point to the left of the lower-right corner.
//
// A loop processes each point in ''next'':
// 1. If this point was already processed, skip it
// 2. If it is possible to move right then get the number of routes stored in ''grid[x+1][y]''
// 3. If it is possible to move down then get the number of routes stored in ''grid[x][y+1]''
// 4. Write the sum of step 2 and 3 to ''grid[x][y]'', potentially avoid overflow (see Hackerrank modification)
// 5. If there is a left neighbor, enqueue it in ''next''
// 6. If there is a neighbor above, enqueue it in ''next''
//
// Finally we have the result in ''grid[0][0]''.
//
// # Alternative
// I later found a posting on the internet that the result is
//
// `dfrac{(width + height)!}{width! height!}`
//
// but I'm not a math guy, I'm a programmer ...
// however, the best explanation for that formula is as follows and unfortunately, it's about bits :-) :
//
// If we take any route through the grid then a decision has to be made at each point: walk to the right or walk down.
// That's similar to a binary number where each bit can be either 0 or 1. Let's say 0 means "right" and 1 means "down".
//
// Then we have to make `width + height` decisions: exactly `width` times we have to go right and exactly `height` times we have to go down.
// In our imaginary binary number, there are `width + height` bits with `height` times 0s and `width` times 1s.
//
// All permutations of these bits are valid routes in the grid.
//
// And as turns out, there are `dfrac{(width + height)!}{width! height!}` permutations, see https://en.wikipedia.org/wiki/Permutation
//
// # Hackerrank
// The result should be printed ` mod (10^9 + 7)`

#include <vector>
#include <deque>
#include <utility>
#include <iostream>

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int width, height;
    std::cin >> width >> height;

    // create a 2D array which contains the number of paths
    // from the current lattice point to the lower-right corner
    // there are (width + 1) * (height + 1) such points
    // for the original problem, i.e. 21x21 numbers must be found
    const unsigned long long Unknown = 0;
    std::vector<std::vector<unsigned long long>> grid(width + 1);
    for (auto& column : grid)
      column.resize(height + 1, Unknown);

    // one route if we are already at the goal
    grid[width][height] = 1;

    // enqueue the next unprocessed lattice points: left and upper neighbor of the lower-right corner
    std::deque<std::pair<unsigned int, unsigned int>> next;
    next.push_back(std::make_pair(width - 1, height));
    next.push_back(std::make_pair(width, height - 1));

    // as long as there are unprocessed points
    while (!next.empty())
    {
      // get next point
      auto current = next.front();
      next.pop_front();

      // I prefer names which are easier to read ...
      auto x = current.first;
      auto y = current.second;

      // already solved ?
      if (grid[x][y] != Unknown)
        continue;

      // sum of all path when going right plus when going down
      unsigned long long routes = 0;
      if (x < width)  // can go right ?
        routes += grid[x + 1][y];
      if (y < height) // can go down ?
        routes += grid[x][y + 1];

#define ORIGINAL
#ifndef ORIGINAL
      routes %= 1000000007; // Hackerrank wants the result MOD 10^9 + 7
#endif
      // solved number for current lattice point
      grid[x][y] = routes;

      // add left and upper neighbors for further processing
      if (x > 0)
        next.push_back(std::make_pair(x - 1, y));
      if (y > 0)
        next.push_back(std::make_pair(x, y - 1));
    }

    // we are done !
    std::cout << grid[0][0] << std::endl;
  }
  return 0;
}
