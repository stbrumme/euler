// ////////////////////////////////////////////////////////
// # Title
// Langton's ant
//
// # URL
// https://projecteuler.net/problem=349
// http://euler.stephan-brumme.com/349/
//
// # Problem
// An ant moves on a regular grid of squares that are coloured either black or white.
// The ant is always oriented in one of the cardinal directions (left, right, up or down) and moves from square to adjacent square according to the following rules:
// - if it is on a black square, it flips the color of the square to white, rotates 90 degrees counterclockwise and moves forward one square.
// - if it is on a white square, it flips the color of the square to black, rotates 90 degrees clockwise and moves forward one square.
//
// Starting with a grid that is entirely white, how many squares are black after `10^18` moves of the ant?
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I had no idea how to solve this problem until I read the Wikipedia page about Langton's ant: https://en.wikipedia.org/wiki/Langton%27s_ant
// One thing was especially interesting: the initial movement of the ant may be chaotic, but after a while a certain pattern develops which repeats after 104 steps.
//
// My program simulates the movement of the ant on a 64x64 grid (starting in the middle).
// It counts the number of black squares and every 104 steps (= 1 cycle) it computes the delta compared to the number of black squares 104 steps ago.
// As soon as I observe the same difference over at least 10 cycles I can easily find out how many cycles are needed for `10^18` steps (minus the steps already taken).
//
// To simplify my computation I don't check the difference at the start of a cycle but after the 40th steps of a cycle because `10^18 mod 104 = 40`.
//
// # Note
// The recurring pattern appears after about 10000 steps.

#include <iostream>
#include <vector>

int main()
{
  auto limit = 1000000000000000000LL;
  std::cin >> limit;

  // colors encoded with a single bit
  const bool White = false;
  const bool Black = true;

  // 128x128 squares (nice power of two with sufficient squares for the first 10000 steps)
  const unsigned int Size = 128;

  // 2D grid stored as a 1D array: pos = y * Size + x;
  std::vector<bool> grid(Size * Size, White);

  // initial position of the ant
  unsigned int x = Size / 2;
  unsigned int y = Size / 2;

  // delta movement when ant moves Up, Left, Down, Right
  const short DeltaX[] = {  0, +1,  0, -1 };
  const short DeltaY[] = { +1,  0, -1,  0 };
  // direction 0..3 (see DeltaX and DeltaY)
  short direction = 0; // any direction is fine

  // a pattern with cycle length 104 emerges after a while (see https://en.wikipedia.org/wiki/Langton%27s_ant )
  const auto Cycle     = 104;
  const auto Remainder = limit % Cycle; // = 40

  // number of black squares
  auto count = 0LL;
  // and the value of "count" 104 steps ago
  auto last = count;

  // keep track of the deltas (which are "count-last")
  std::vector<int> lastDeltas = { 0 };
  // assume a "steady state" if the most recent 10 cycles have the same deltas
  const unsigned int StopIfSameDeltas = 10;

  // number of steps taken
  auto steps = 0;
  while (steps < limit)
  {
    // check every 104 steps whether the "steady state" was achieved
    if (steps % Cycle == Remainder)
    {
      // change of Black squares
      auto diff = int(count - last);
      lastDeltas.push_back(diff);
      last = count;

      // need a few samples ...
      if (lastDeltas.size() >= StopIfSameDeltas)
      {
        bool samesame = true;
        // always the same diff during the most recent cycles ?
        for (auto scan = lastDeltas.rbegin(); scan != lastDeltas.rbegin() + StopIfSameDeltas; scan++)
          if (*scan != diff)
          {
            // nope, found a different difference :-(
            samesame = false;
            break;
          }

        // yes, entered "steady state"
        if (samesame)
        {
          // determine number of cycles left
          auto remainingCycles = (limit - steps) / Cycle;
          // and multiply by 12
          count += remainingCycles * diff;
          // stop searching
          break;
        }
      }
    }

    // ant moves one step
    auto pos = y * Size + x;
    if (grid[pos] == White)
    {
      // flip square
      grid[pos] = Black;
      count++;

      // counter-clockwise
      direction = (direction + 1) % 4;
    }
    else // Black
    {
      // flip square
      grid[pos] = White;
      count--;

      // clockwise
      direction = (direction + 4 - 1) % 4; // plus 4 avoids negative numbers
    }

    // "That's one small step for an ant, one giant leap for antkind"
    x += DeltaX[direction];
    y += DeltaY[direction];
    steps++;
  }

  // display result
  std::cout << count << std::endl;
  return 0;
}
