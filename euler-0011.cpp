// ////////////////////////////////////////////////////////
// # Title
// Largest product in a grid
//
// # URL
// https://projecteuler.net/problem=11
// http://euler.stephan-brumme.com/11/
//
// # Problem
// In the 20x20 grid below, four numbers along a diagonal line have been marked in red.
//
// 08 02 22 97 38 15 00 40 00 75 04 05 07 78 52 12 50 77 91 08
// 49 49 99 40 17 81 18 57 60 87 17 40 98 43 69 48 04 56 62 00
// 81 49 31 73 55 79 14 29 93 71 40 67 53 88 30 03 49 13 36 65
// 52 70 95 23 04 60 11 42 69 24 68 56 01 32 56 71 37 02 36 91
// 22 31 16 71 51 67 63 89 41 92 36 54 22 40 40 28 66 33 13 80
// 24 47 32 60 99 03 45 02 44 75 33 53 78 36 84 20 35 17 12 50
// 32 98 81 28 64 23 67 10 __26__ 38 40 67 59 54 70 66 18 38 64 70
// 67 26 20 68 02 62 12 20 95 __63__ 94 39 63 08 40 91 66 49 94 21
// 24 55 58 05 66 73 99 26 97 17 __78__ 78 96 83 14 88 34 89 63 72
// 21 36 23 09 75 00 76 44 20 45 35 __14__ 00 61 33 97 34 31 33 95
// 78 17 53 28 22 75 31 67 15 94 03 80 04 62 16 14 09 53 56 92
// 16 39 05 42 96 35 31 47 55 58 88 24 00 17 54 24 36 29 85 57
// 86 56 00 48 35 71 89 07 05 44 44 37 44 60 21 58 51 54 17 58
// 19 80 81 68 05 94 47 69 28 73 92 13 86 52 17 77 04 89 55 40
// 04 52 08 83 97 35 99 16 07 97 57 32 16 26 26 79 33 27 98 66
// 88 36 68 87 57 62 20 72 03 46 33 67 46 55 12 32 63 93 53 69
// 04 42 16 73 38 25 39 11 24 94 72 18 08 46 29 32 40 62 76 36
// 20 69 36 41 72 30 23 88 34 62 99 69 82 67 59 85 74 04 36 16
// 20 73 35 29 78 31 90 01 74 31 49 71 48 86 81 16 23 57 05 54
// 01 70 54 71 83 51 54 69 16 92 33 48 61 43 52 01 89 19 67 48
//
// The product of these numbers is 26 x 63 x 78 x 14 = 1788696.
//
// What is the greatest product of four adjacent numbers in the same direction
// (up, down, left, right, or diagonally) in the 20x20 grid?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// For each position of the grid I find the product of 4 connected cells:
// 1. current cell and its three neighbors when going to the right side
// 2. current cell and its three neighbors below it
// 3. current cell and its three neighbors going right and down
// 4. current cell and its three neighbors going left and down
//
// For each of these steps I have to check whether enough neighbor exist.
// And finally the greatest product is printed.
//
// # Note
// Be careful when reading a 2D matrix from console:
// the outer loop must belong to the y-axis, the inner to x.
// A common mistake of mine is to swap those two.

#include <iostream>

int main()
{
  // always a 20x20 matrix
  const unsigned int Size = 20;
  unsigned int matrix[Size][Size];

  // read from console
  for (unsigned int y = 0; y < Size; y++)
    for (unsigned int x = 0; x < Size; x++)
      std::cin >> matrix[x][y];

  unsigned int best = 0;
  // walk through all cells of the matrix
  for (unsigned int y = 0; y < Size; y++)
    for (unsigned int x = 0; x < Size; x++)
    {
      // three more horizontal cells (right)
      if (x + 3 < Size)
      {
        unsigned int current = matrix[x][y] * matrix[x+1][y] * matrix[x+2][y] * matrix[x+3][y];
        if (best < current)
          best = current;
      }
      // three more vertical cells available (down)
      if (y + 3 < Size)
      {
        unsigned int current = matrix[x][y] * matrix[x][y+1] * matrix[x][y+2] * matrix[x][y+3];
        if (best < current)
          best = current;
      }
      // three more diagonal cells (right-down)
      if (x + 3 < Size && y + 3 < Size)
      {
        unsigned int current = matrix[x][y] * matrix[x+1][y+1] * matrix[x+2][y+2] * matrix[x+3][y+3];
        if (best < current)
          best = current;
      }
      // three more diagonal cells (left-down)
      if (x + 3 < Size && y >= 3)
      {
        unsigned int current = matrix[x][y] * matrix[x+1][y-1] * matrix[x+2][y-2] * matrix[x+3][y-3];
        if (best < current)
          best = current;
      }
    }

  std::cout << best << std::endl;
  return 0;
}
