// ////////////////////////////////////////////////////////
// # Title
// Flea Circus
//
// # URL
// https://projecteuler.net/problem=213
// http://euler.stephan-brumme.com/213/
//
// # Problem
// A 30x30 grid of squares contains 900 fleas, initially one flea per square.
// When a bell is rung, each flea jumps to an adjacent square at random (usually 4 possibilities, except for fleas on the edge of the grid or at the corners).
//
// What is the expected number of unoccupied squares after 50 rings of the bell? Give your answer rounded to six decimal places.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Assume there is a 3x2 with two fleas A and B:
// || 4 || 4 || 4 ||
// || A ++ . ++ . ||
// || . ++ B ++ . ||
//
// The probability `land_{A,1}(x,y)` of flea A to land on a certain square after one jump is:
// ||  4  ||  4  ||  4  ||
// || 0   ++ 0.5 ++ 0   ||
// || 0.5 ++ 0   ++ 0   ||
// Note: Flea A had 2 options when it started at square (1,2), therefore `directions(1,2) = 100% / 2 = 50% = 0.5`.
//
// The probability `land_{B,1}(x,y)` of flea B to land on a certain square after one jump is:
// ||  4    ||  4    ||  4    ||
// || 0     ++ 0.333 ++ 0     ||
// || 0.333 ++ 0     ++ 0.333 ||
// Note: Flea B had 3 options when it started at square (2,1), therefore `directions(2,1) = 100% / 3 = 33.3% = 0.333`.
//
// To find the probability of flea A to be __NOT__ on a certain square I have to compute `empty_{A,1}(x,y) = 1 - land_{A,1}(x,y)`:
// ||  4  ||  4  ||  4  ||
// || 1   ++ 0.5 ++ 1   ||
// || 0.5 ++ 1   ++ 1   ||
//
// And the same for flea B: `empty_{B,1}(x,y) = 1 - land_{B,1}(x,y)`:
// ||  4    ||  4    ||  4    ||
// || 1     ++ 0.666 ++ 1     ||
// || 0.666 ++ 1     ++ 0.666 ||
//
// A square is empty is neither flea A nor flea B landed on it: `empty_1(x,y) = empty_{A,1}(x,y) * empty_{B,1}(x,y)`:
// ||  4    ||  4    ||  4    ||
// || 1     ++ 0.333 ++ 1     ||
// || 0.333 ++ 1     ++ 0.666 ||
//
// So the expected total number of empty squares after one jump is `sum{empty_1} = 1 + 0.333 + 1 + 0.333 + 1 + 0.666 = 4.333`.
// There were exactly four empty squares before the first jump but there are on average 4.333 empty squares after one jump (==> plus 0.333)
// because it's possible that both fleas land on the same square, leaving five square empty instead of four.
//
// In the second iteration, the probability `land_{A,2}(x,y)` of flea A depends on the probability of being on the square in the previous iteration, too.
// The probabilities `land_{A,2}(x,y)` of flea A after two jumps are:
// ||  4    ||  4     ||  4    ||
// || 0.416 ++ 0      ++ 0.166 ||
// || 0     ++ 0.416* ++ 0     ||
//
// Let's examine the square `land_{A,2}(2,1)` which is denoted by a star:
// `land_{A,2}(2,1) = land_{A,1}(2,2) * directions(2,2) + land_{A,1}(1,1) * directions(1,1) = 0.5 * 0.333 + 0.5 * 0.5 = 0.416`.
//
// ## Implementation
// Initially all squares are empty, therefore ''empty[x][y] = 1''.
//
// Then my program tracks each a single flea (such as flea A or B) over 50 rounds.
// It has a grid ''current'' which is intially zero and contains a single 1 at the start position of the flea.
// After each jump, the probabilities of each square the flea can jump to are updated and stored in ''next''.
// If all squares are processed, ''next'' is copied to ''current''.
// After 50 rounds, the "emptiness" of each square is ''empty[x][y] *= 1 - current[x][y]''.
//
// This process is repeated for all 30x30 fleas after which the sum of all squares in ''empty'' is the result.
//
// # Note
// The grid offers several opportunies to exploit symmetries.
// I mirror along the x- and/or y-axis if possible (see ''#define FAST_SYMMETRY'').
// In the end I get a 3x speedup.
//
// You could mirror along the diagonals, too, but then the code might get messy.
//
// # Alternative
// Even though the algorithm is pretty simple, I struggled with a few details and couldn't find the correct answer for a while.
// Therefore I wrote a Monte-Carlo simulation "to get a feeling" for the result.
// Obviously, the Monte-Carlo simulation wasn't able to compute an exact result, even after millions of repetitions.
// But it helped me to spot my errors and fix them.

#include <iostream>
#include <iomanip>
#include <vector>

// size of the flea circus
unsigned int width  = 30;
unsigned int height = 30;
// number of jumps for each flea
unsigned int rounds = 50;

// a fixed 2D array
typedef std::vector<std::vector<double>> Grid;
// it looks weird, but the inner array describes the second index

// create a 2D (width x height) and set all elements to initialValue
Grid makeGrid(double initialValue)
{
  Grid result(width);
  for (auto& row : result)
    row.resize(height, initialValue);
  return result;
}

int main()
{
  // read parameters
  std::cin >> width >> height >> rounds;

  // probability that squares are empty
  // ==> no fleas yet, therefore "emptiness" probability is 1 (=100%) for each square
  Grid empty = makeGrid(1);

  // use symmetry to speed up the process
  bool mirrorX = false;
  bool mirrorY = false;
  unsigned int maxX = width;
  unsigned int maxY = height;
#define FAST_SYMMETRY
#ifdef FAST_SYMMETRY
  if (width  % 2 == 0)
  {
    mirrorX = true;
    maxX = width  / 2;
  }
  if (height % 2 == 0)
  {
    mirrorY = true;
    maxY = height / 2;
  }
#endif

  // analyze each flea
  for (unsigned int startX = 0; startX < maxX; startX++)
    for (unsigned int startY = 0; startY < maxY; startY++)
    {
      // drop a single flea at (x,y):
      // probability of seeing the flea is zero for each square ...
      Grid current = makeGrid(0);
      // ... except for the square where it starts
      current[startX][startY] = 1;

      // trace probability of landing at each square after a few rounds
      for (unsigned int round = 0; round < rounds; round++)
      {
        // track probability of seeing the flea after one more jump
        Grid next = makeGrid(0);

        // look at each square from the previous round
        for (unsigned int x = 0; x < width; x++)
          for (unsigned int y = 0; y < height; y++)
          {
            // flea couldn't be there ? ==> skip
            if (current[x][y] == 0)
              continue;

            // count number of possible destination squares
            unsigned int directions = 4;
            if (x == 0 || x == width - 1)
              directions--;
            if (y == 0 || y == height - 1)
              directions--;

            // probability of landing on a square is
            // "probability of starting at current square" / "number of options"
            double probability = current[x][y] / directions;

            // add probability to each allowed destination square
            if (x > 0)
              next[x - 1][y] += probability;
            if (x < width - 1)
              next[x + 1][y] += probability;
            if (y > 0)
              next[x][y - 1] += probability;
            if (y < height - 1)
              next[x][y + 1] += probability;
          }

        current = std::move(next);
      }

      // 1. current[x][y] contains the probability that our flea is on square (x,y) after xx rounds
      // 2. the probability of being empty is the opposite, it's: 1 - current[x][y]
      // 3. and the probability that no flea finishes on that square (no matter where it started)
      //    is the product of the "emptiness" probabilities of all fleas
      for (unsigned int x = 0; x < width; x++)
        for (unsigned int y = 0; y < height; y++)
        {
          empty[x][y] *= 1 - current[x][y];

          // use symmetries
          if (mirrorX)
            empty[x][y] *= 1 - current[width - 1 - x][y];
          if (mirrorY)
            empty[x][y] *= 1 - current[x][height - 1 - y];
          if (mirrorX && mirrorY)
            empty[x][y] *= 1 - current[width - 1 - x][height - 1 - y];
        }
    }

  // sum of all probabilities
  double result = 0;
  for (unsigned int x = 0; x < width; x++)
    for (unsigned int y = 0; y < height; y++)
      result += empty[x][y];

  // display result
  std::cout << std::fixed << std::setprecision(6) << result << std::endl;
  return 0;
}
