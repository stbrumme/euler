// ////////////////////////////////////////////////////////
// # Title
// Path sum: four ways
//
// # URL
// https://projecteuler.net/problem=83
// http://euler.stephan-brumme.com/83/
//
// # Problem
// In the 5 by 5 matrix below, the minimal path sum from the top left to the bottom right, by moving left, right, up, and down,
// is indicated in bold and is equal to 2297.
//
// ''__131__ 673 __234__ __103__  __18__''
// ''__201__  __96__ __342__ 965 __150__''
// ''630 803 746 __422__ 111''
// ''537 699 497 __121__ 956''
// ''805 732 524  __37__ __331__''
//
// Find the minimal path sum, in [matrix.txt](https://projecteuler.net/project/resources/p083_matrix.txt) (right click and "Save Link/Target As..."), a 31K text file containing a 80 by 80 matrix, from the top left to the bottom right by moving left, right, up, and down.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// My code is almost identical to problem 81.
//
// Relevant changes:
// - add the upper and bottom neighbor to the priority queue, too
// - and that's it !

#include <queue>
#include <vector>
#include <iostream>

// 2D matrix: unfortunately x and y are swapped, so we need to write matrix[y][x]
// instead of the more common matrix[x][y]
typedef std::vector<std::vector<unsigned int>> Matrix;

// use a priority queue to find the next cell to process
struct Cell
{
  // position
  unsigned int x, y;
  // sum of shortest route so far
  unsigned long long weight;

  Cell(unsigned int x_, unsigned int y_, unsigned long long weight_)
  : x(x_), y(y_), weight(weight_) {}

  // std::priority_queue returns the LARGEST element, therefore I implement this function "the other way 'round"
  bool operator<(const Cell& cell) const
  {
      return weight > cell.weight; // ">" is not a typo !
  }
};

// breadth-search
unsigned long long search(const Matrix& matrix)
{
  // matrix height/width
  const auto size = matrix.size();

  // store already processed cells
  std::vector<std::vector<bool>> processed(matrix.size());
  for (auto& row : processed)
    row.resize(matrix.size(), false);

  // process cells in increasing distance from starting point
  std::priority_queue<Cell> next;
  // add starting point (upper left corner)
  next.push(Cell(0, 0, matrix[0][0]));

  while (!next.empty())
  {
    // get cell with the smallest weight
    Cell cell = next.top();
    // and remove it from the queue
    next.pop();

    // we have been here before ?
    // must have been on a shorter route, hence discard current cell
    if (processed[cell.y][cell.x])
      continue;

    processed[cell.y][cell.x] = true;

    // finished ?
    if (cell.x == size - 1 && cell.y == size - 1)
      return cell.weight;

    // one step right
    if (cell.x + 1 < size)
      next.push(Cell(cell.x + 1, cell.y, cell.weight + matrix[cell.y][cell.x + 1]));
    // one step down
    if (cell.y + 1 < size)
      next.push(Cell(cell.x, cell.y + 1, cell.weight + matrix[cell.y + 1][cell.x]));
    // one step up
    if (cell.y > 0)
      next.push(Cell(cell.x, cell.y - 1, cell.weight + matrix[cell.y - 1][cell.x]));
    // one step left
    if (cell.x > 0)
      next.push(Cell(cell.x - 1, cell.y, cell.weight + matrix[cell.y][cell.x - 1]));
  }

  return -1; // failed
}

int main()
{
  unsigned int size = 80;
//#define ORIGINAL
#ifndef ORIGINAL
  std::cin >> size;
#endif

  Matrix matrix(size);
  for (auto& row : matrix)
  {
    row.resize(size);
    for (auto& cell : row)
    {
#ifdef ORIGINAL
      // unfortunately, Project Euler used a CSV format which is a bit tricky to parse in C++
      cell = 0;
      // read until the number is complete or we run out of input
      while (std::cin)
      {
        char c;
        std::cin.get(c);
        // number complete ?
        if (c < '0' || c > '9')
          break;

        // add digit to current number
        cell *= 10;
        cell += c - '0';
      }
#else
      std::cin >> cell;
#endif
    }
  }

  // go !
  std::cout << search(matrix) << std::endl;

  return 0;
}
