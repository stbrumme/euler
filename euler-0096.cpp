// ////////////////////////////////////////////////////////
// # Title
// Su Doku
//
// # URL
// https://projecteuler.net/problem=96
// http://euler.stephan-brumme.com/96/
//
// # Problem
// Su Doku (Japanese meaning number place) is the name given to a popular puzzle concept.
// Its origin is unclear, but credit must be attributed to Leonhard Euler who invented a similar, and much more difficult, puzzle idea called Latin Squares.
// The objective of Su Doku puzzles, however, is to replace the blanks (or zeros) in a 9 by 9 grid in such that each row, column, and 3 by 3 box contains each of the digits 1 to 9.
// Below is an example of a typical starting puzzle grid and its solution grid.
//
// || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 ||
// || 0 ++ 0 ++ 3 ++ 0 ++ 2 ++ 0 ++ 6 ++ 0 ++ 0 ||
// || 9 ++ 0 ++ 0 ++ 3 ++ 0 ++ 5 ++ 0 ++ 0 ++ 1 ||
// || 0 ++ 0 ++ 1 ++ 8 ++ 0 ++ 6 ++ 4 ++ 0 ++ 0 ||
// || 0 ++ 0 ++ 8 ++ 1 ++ 0 ++ 2 ++ 9 ++ 0 ++ 0 ||
// || 7 ++ 0 ++ 0 ++ 0 ++ 0 ++ 0 ++ 0 ++ 0 ++ 8 ||
// || 0 ++ 0 ++ 6 ++ 7 ++ 0 ++ 8 ++ 2 ++ 0 ++ 0 ||
// || 0 ++ 0 ++ 2 ++ 6 ++ 0 ++ 9 ++ 5 ++ 0 ++ 0 ||
// || 8 ++ 0 ++ 0 ++ 2 ++ 0 ++ 3 ++ 0 ++ 0 ++ 9 ||
// || 0 ++ 0 ++ 5 ++ 0 ++ 1 ++ 0 ++ 3 ++ 0 ++ 0 ||
//
// || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 ||
// || 4 ++ 8 ++ 3 ++ 9 ++ 2 ++ 1 ++ 6 ++ 5 ++ 7 ||
// || 9 ++ 6 ++ 7 ++ 3 ++ 4 ++ 5 ++ 8 ++ 2 ++ 1 ||
// || 2 ++ 5 ++ 1 ++ 8 ++ 7 ++ 6 ++ 4 ++ 9 ++ 3 ||
// || 5 ++ 4 ++ 8 ++ 1 ++ 3 ++ 2 ++ 9 ++ 7 ++ 6 ||
// || 7 ++ 2 ++ 9 ++ 5 ++ 6 ++ 4 ++ 1 ++ 3 ++ 8 ||
// || 1 ++ 3 ++ 6 ++ 7 ++ 9 ++ 8 ++ 2 ++ 4 ++ 5 ||
// || 3 ++ 7 ++ 2 ++ 6 ++ 8 ++ 9 ++ 5 ++ 1 ++ 4 ||
// || 8 ++ 1 ++ 4 ++ 2 ++ 5 ++ 3 ++ 7 ++ 6 ++ 9 ||
// || 6 ++ 9 ++ 5 ++ 4 ++ 1 ++ 7 ++ 3 ++ 8 ++ 2 ||
//
// A well constructed Su Doku puzzle has a unique solution and can be solved by logic, although it may be necessary
// to employ "guess and test" methods in order to eliminate options (there is much contested opinion over this).
// The complexity of the search determines the difficulty of the puzzle; the example above is considered easy because it can be solved by straight forward direct deduction.
//
// The 6K text file, [sudoku.txt](https://projecteuler.net/project/resources/p096_sudoku.txt) (right click and 'Save Link/Target As...'), contains fifty different Su Doku puzzles ranging in difficulty,
// but all with unique solutions (the first puzzle in the file is the example above).
//
// By solving all fifty puzzles find the sum of the 3-digit numbers found in the top left corner of each solution grid;
// for example, 483 is the 3-digit number found in the top left corner of the solution grid above.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// My program uses backtracking to ''solve'' a Su Doku: it looks for the first empty cell (which contains a zero).
// Then it scans the row, column and the cell's 3x3 box to figure out which numbers could be placed in the cell (see ''available'').
// The algorithm tries all ''available'' numbers and recursively calls itself.
// If no number can be placed in the current empty cell, then the algorithm failed and has to backtrack to the previous cell (return ''false'').
// If no empty cell can be found, then the Su Doku was solved (return ''true'').
//
// To speed up the program, the board is modified in-place.
//
// # Hackerrank
// Just show the full solution of the current board.

#include <string>
#include <iostream>

typedef unsigned int Board[9][9];
const unsigned int Empty = 0;

// find the first solution and store it in board
bool solve(Board& board)
{
  for (unsigned int y = 0; y < 9; y++)
    for (unsigned int x = 0; x < 9; x++)
    {
      // already solved cell ?
      if (board[x][y] != Empty)
        continue;

      // track which numbers could be placed in the current cell
      bool available[9+1] = { false,  true, true, true, true, true, true, true, true, true };
      // note: available[0] is a dummy entry, the program only uses available[1..9]

      // same row and column
      for (unsigned int i = 0; i < 9; i++)
      {
        if (board[i][y] != Empty)
          available[board[i][y]] = false;
        if (board[x][i] != Empty)
          available[board[x][i]] = false;
      }
      // same region (3x3 area)
      unsigned int rx = (x / 3) * 3;
      unsigned int ry = (y / 3) * 3;
      for (unsigned int i = 0; i < 3; i++)
        for (unsigned int j = 0; j < 3; j++)
          if (board[i + rx][j + ry] != Empty)
            available[board[i + rx][j + ry]] = false;

      // try all still available numbers
      for (unsigned int i = 1; i <= 9; i++)
        if (available[i])
        {
          board[x][y] = i;
          if (solve(board))
            return true;
        }
      // all failed, restore old board
      board[x][y] = Empty;
      return false;
    }

  // solved it
  return true;
}

int main()
{
//#define ORIGINAL
#ifdef ORIGINAL
  unsigned int tests = 50;
  unsigned int sum   =  0;
#else
  unsigned int tests =  1;
#endif

  while (tests--)
  {
#ifdef ORIGINAL
    // skip labels "GRID xy"
    std::string dummy;
    std::cin >> dummy >> dummy;
#endif

    // read board
    Board board;
    for (unsigned int y = 0; y < 9; y++)
    {
      std::string line;
      std::cin >> line;
      for (unsigned int x = 0; x < 9; x++)
        board[x][y] = line[x] - '0';
    }

    // and replace all zeros (=Empty) with proper digits
    solve(board);

#ifdef ORIGINAL
    // the first the cells
    sum += 100 * board[0][0] + 10 * board[1][0] + board[2][0];
#else
    // print full solution
    for (unsigned int y = 0; y < 9; y++)
    {
      for (unsigned int x = 0; x < 9; x++)
        std::cout << board[x][y];
      std::cout << std::endl;
    }
#endif
  }

#ifdef ORIGINAL
  std::cout << sum;
#endif

  return 0;
}
