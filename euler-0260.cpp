// ////////////////////////////////////////////////////////
// # Title
// Stone Game
//
// # URL
// https://projecteuler.net/problem=260
// http://euler.stephan-brumme.com/260/
//
// # Problem
// A game is played with three piles of stones and two players.
// At her turn, a player removes one or more stones from the piles.
// However, if she takes stones from more than one pile, she must remove the same number of stones from each of the selected piles.
//
// In other words, the player chooses some `N>0` and removes:
//
//     `N` stones from any single pile; or
//     `N` stones from each of any two piles (`2N` total); or
//     `N` stones from each of the three piles (`3N` total).
//
// The player taking the last stone(s) wins the game.
//
// A winning configuration is one where the first player can force a win.
// For example, (0,0,13), (0,11,11) and (5,5,5) are winning configurations because the first player can immediately remove all stones.
//
// A losing configuration is one where the second player can force a win, no matter what the first player does.
// For example, (0,1,2) and (1,3,3) are losing configurations: any legal move leaves a winning configuration for the second player.
//
// Consider all losing configurations `(x_i,y_i,z_i)` where `x_i <= y_i <= z_i <= 100`.
// We can verify that `sum{x_i+y_i+z_i} = 173895` for these.
//
// Find `sum{x_i+y_i+z_i}` where `(x_i,y_i,z_i)` ranges over the losing configurations with  `x_i <= y_i <= z_i <= 1000`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Every position `(0,0,s)` is won because the current player will take all `s` stones from the third pile and win immediately.
// Every position `(0,s,s)` is won because the current player will take all `s` stones from the second and from the third pile and win immediately.
// Every position `(s,s,s)` is won because the current player will take all `s` stones from all three piles and win immediately.
// More specifically, these positions are won because there is a transition from the current state to the final state `(0,0,0)`.
//
// If the player to move has position `(x_i,y_i,z_i)` then he/she must avoid to take stones in such a way that his/her opponent has a won position.
// For example, taking 3 stones from the largest pile of `(0,2,5)` will yield `(0,2,2)` and the opponent can will immediately.
//
// A position is lost when all transitions lead to a position where the opponent wins.
// A position is won when there is at least one transition leading to a position where the opponent loses.
//
// I only need to consider moves where one pile becomes empty:
// - if that is a winning position, then the current player will play that move
// - if it turns out to be a losing position, then the opponent will enforce that move by mirroring
//
// My program walks through all possible pile sizes `x <= y <= z` and looks at every potential move which "empties" one or more piles.
// Only if all of them lead to a won position (==> the opponent wins), then ''count'' is incremented by ''x + y + z''.
//
// # Alternatives
// The logic becomes much simples if I replace the three containers ''one'', ''two'' and ''all'' by a single container with three indices.
// Unfortunately it needs `1001^3 approx 1` trillion (= is 128 MByte RAM); my solution needs less than 0.5 MByte and typically runs inside the L2/L3 CPU cache.

#include <iostream>
#include <vector>

unsigned int maxPileSize = 1000;

// compute linear index from two offsets
unsigned int id(unsigned int a, unsigned int b)
{
  return a * (maxPileSize + 1) + b;
}

int main()
{
  std::cin >> maxPileSize;

  // the current position is only lost if every possible move leads to a position
  // where the opponent wins
  // that means: no matter how many stones I pick from one, two or three piles,
  //             the follow-up state must not be lost

  const bool Won  = false;
  const bool Lost = true;

  // assume each position is potentially won - unless proven otherwise
  std::vector<bool> one((maxPileSize + 1) * (maxPileSize + 1), Won);
  std::vector<bool> two((maxPileSize + 1) * (maxPileSize + 1), Won);
  std::vector<bool> all((maxPileSize + 1) * (maxPileSize + 1), Won);

  // count x+y+z for each lost position
  unsigned int count = 0;

  for (unsigned int x = 0; x <= maxPileSize; x++)
    for (unsigned int y = x; y <= maxPileSize; y++)
    {
      // copy this condition from the inner loop (and keep there for the sake of completeness, though it's redundant)
      if (one[id(x,y)] == Lost)
        continue;

      for (unsigned int z = y; z <= maxPileSize; z++)
      {
        // can current player win by clearing one pile ?
        // check if (0,y,z) or (x,0,z) or (x,y,0) wins
        // take x stones from first  pile: check (x-x,  y,   z ) = (0, y, z)
        // take y stones from second pile: check ( x,  y-y,  z ) = (x, 0, z)
        // take z stones from third  pile: check ( x,   y,  z-z) = (x, y, 0)
        if (one[id(y,z)] == Lost ||
            one[id(x,z)] == Lost ||
            one[id(x,y)] == Lost)
          continue; // opponent can be forced into a lost position

        // can current player win by clearing one pile
        // and removing the same number of stones from a second pile ?
        // take x stones from first and second pile: check (x-x, y-x,  z ) = (0, y-x,  z )
        // take y stones from first and second pile: check ( x,  y-y, z-y) = (x,  0,  z-y)
        // take x stones from first and third  pile: check (x-x,  y,  z-x) = (0,  y,  z-x)
        // (note that x <= y <= z, therefore can't take z stones)
        if (two[id(y-x, z)] == Lost ||
            two[id(z-y, x)] == Lost ||
            two[id(z-x, y)] == Lost)
          continue; // opponent can be forced into a lost position

        // can current player win by clearing the first pile
        // and removing the same number of stones from the second and third pile ?
        // note that x <= y <= z
        // take x stones from all piles: check (x-x, y-x, z-x) = (0, y-x, z-x)
        if (all[id(y-x, z-x)] == Lost)
          continue; // opponent can be forced into a lost position

        // current player lost, opponents wins all follow-up positions
        count += x + y + z;

        // mark all reachable positions as lost
        // (same indices and arrays as used in the if-conditions above)
        one[id(y, z)] = Lost;
        one[id(x, z)] = Lost;
        one[id(x, y)] = Lost;

        two[id(y-x, z)] = Lost;
        two[id(z-y, x)] = Lost;
        two[id(z-x, y)] = Lost;

        all[id(y-x, z-x)] = Lost;
      }
    }

  // and done !
  std::cout << count << std::endl;
  return 0;
}
