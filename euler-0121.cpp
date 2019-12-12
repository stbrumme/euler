// ////////////////////////////////////////////////////////
// # Title
// Disc game prize fund
//
// # URL
// https://projecteuler.net/problem=121
// http://euler.stephan-brumme.com/121/
//
// # Problem
// A bag contains one red disc and one blue disc. In a game of chance a player takes a disc at random and its colour is noted.
// After each turn the disc is returned to the bag, an extra red disc is added, and another disc is taken at random.
//
// The player pays &pound;1 to play and wins if they have taken more blue discs than red discs at the end of the game.
//
// If the game is played for four turns, the probability of a player winning is exactly 11/120, and so the maximum prize fund the banker
// should allocate for winning in this game would be &pound;10 before they would expect to incur a loss.
// Note that any payout will be a whole number of pounds and also includes the original &pound;1 paid to play the game, so in the example given the player actually wins &pound;9.
//
// Find the maximum prize fund that should be allocated to a single game in which fifteen turns are played.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// I compute all possible states after each round.
// Before the first round, the player had obviously no red and no blue disc. This ''State'' was the only ''possibility''.
// In each round, the number of possible combinations are `possibilities_n = possibilities_{n-1} * (red + blue)` because the player picks any disc at random.
// Several choices lead to the same next state, e.g. it doesn't matter which red disc he/she picks.
// Therefore the total number of states increases by one per round but the possibilities explode in a factorial way.
//
// The final prize is `\lfloor dfrac{sum{possibilities}}{sum{possibilities(blue>red)}} \rfloor`, e.g. `\lfloor dfrac{120}{11} \rfloor = 10` ==> &pound;10 payout.
//
// # Note
// My states don't need to store ''red'' and ''blue'' because they can be derived from the index of the ''State'' object in the ''open'' container.
// I kept those member variables for clarity, though.
//
// # Hackerrank
// Solving the last test case requires more than 40 rounds. That's beyond the reach of ''unsigned long long'' and/or ''double''.
// 19 rounds are fine with my current implemenation. Switching to ''double'' works surprisingly well up to 30 rounds.

#include <iostream>
#include <vector>

typedef unsigned long long Possibilities; // replace unsigned long long by double to compute up to 30 rounds

struct State
{
  // picked red and blue disc
  unsigned int red;
  unsigned int blue;
  // number of different choices leading to this state
  Possibilities possibilities;
};

int main()
{
  unsigned int tests = 1;
  std::cin >> tests;

  while (tests--)
  {
    unsigned int maxRounds = 15;
    std::cin >> maxRounds;

    // number of red and blue discs in the bag
    unsigned int availableRed   = 1;
    unsigned int availableBlue  = 1;
    Possibilities possibilities = 1;

    // initially no disc taken yet
    std::vector<State> open = { { 0, 0, 1 } };

    // evaluateall 15 rounds
    for (unsigned int round = 1; round <= maxRounds; round++)
    {
      // total number of possible choices
      possibilities *= availableRed + availableBlue;

      // set number of blue and red discs per state
      std::vector<State> next;
      for (unsigned int blue = 0; blue <= round; blue++)
      {
        State state;
        state.red  = round - blue;
        state.blue = blue;
        state.possibilities = 0;
        next.push_back(state);
      }

      // update possibilities of each state
      for (auto current : open)
      {
        // picking red
        next[current.blue    ].possibilities += current.possibilities * availableRed;
        // picking blue
        next[current.blue + 1].possibilities += current.possibilities * availableBlue;
      }

      // prepare next iteration
      open = next;
      availableRed++;
    }

    // compute chance that blue > red
    Possibilities moreBlue = 0;
    for (auto current : open)
      if (current.blue > current.red)
        moreBlue += current.possibilities;

    // chance of winning is moreBlue / possibilities, therefore maximum prize is the inverse
    unsigned int prize = possibilities / moreBlue; // integer division !
    std::cout << prize << std::endl;
  }

  return 0;
}
