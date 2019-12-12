// ////////////////////////////////////////////////////////
// # Title
// Monopoly odds
//
// # URL
// https://projecteuler.net/problem=84
// http://euler.stephan-brumme.com/84/
//
// # Problem
// In the game, Monopoly, the standard board is set up in the following way:
//
// ![board](board.gif)
//
// A player starts on the GO square and adds the scores on two 6-sided dice to determine the number of squares they advance in a clockwise direction.
// Without any further rules we would expect to visit each square with equal probability: 2.5%.
// However, landing on G2J (Go To Jail), CC (community chest), and CH (chance) changes this distribution.
//
// In addition to G2J, and one card from each of CC and CH, that orders the player to go directly to jail, if a player rolls three consecutive doubles,
// they do not advance the result of their 3rd roll. Instead they proceed directly to jail.
//
// At the beginning of the game, the CC and CH cards are shuffled. When a player lands on CC or CH they take a card from the top of the respective pile and,
// after following the instructions, it is returned to the bottom of the pile.
// There are sixteen cards in each pile, but for the purpose of this problem we are only concerned with cards that order a movement;
// any instruction not concerned with movement will be ignored and the player will remain on the CC/CH square.
//
// - Community Chest (2/16 cards):
//   Advance to GO, Go to JAIL
//
// - Chance (10/16 cards):
//   Advance to GO, Go to JAIL,
//   Go to C1, Go to E3, Go to H2, Go to R1,
//   Go to next R (railway company), Go to next R, Go to next U (utility company),
//   Go back 3 squares
//
// The heart of this problem concerns the likelihood of visiting a particular square. That is, the probability of finishing at that square after a roll.
// For this reason it should be clear that, with the exception of G2J for which the probability of finishing on it is zero, the CH squares will have the lowest probabilities,
// as 5/8 request a movement to another square, and it is the final square that the player finishes at on each roll that we are interested in.
// We shall make no distinction between "Just Visiting" and being sent to JAIL, and we shall also ignore the rule about requiring a double to "get out of jail",
// assuming that they pay to get out on their next turn.
//
// By starting at GO and numbering the squares sequentially from 00 to 39 we can concatenate these two-digit numbers to produce strings that correspond with sets of squares.
//
// Statistically it can be shown that the three most popular squares, in order, are JAIL (6.24%) = Square 10, E3 (3.18%) = Square 24, and GO (3.09%) = Square 00.
// So these three most popular squares can be listed with the six-digit modal string: 102400.
//
// If, instead of using two 6-sided dice, two 4-sided dice are used, find the six-digit modal string.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Disclaimer: this was one of my least favorite problems so far.
//
// The main idea is to run a Monte-Carlo simulation (see https://en.wikipedia.org/wiki/Monte_Carlo_method ):
// roll the dice five million times and count how often you land on each field.
// The longer the simulation runs, the closer you get to the true statistical value.
//
// Because I hate this problem so much, there is no further explanation of my code ...
//
// # Hackerrank
// You can't solve this problem with my approach: only a true mathematical analysis will give the correct order of all fields
// (yes, some of their test cases ask for __all__ 40 fields in descendingly sorted order).
// Right now I am not interesting in rewriting my code. Apparently, Markov chains are the way to go (see https://en.wikipedia.org/wiki/Markov_chain ).
//
// # Note
// I reduced ''Rolls'' from the original 5 million to 1 million otherwise my live tests would time out.
// It still solves the original problem but has a lower score at Hackerrank.

#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <cmath>

int main()
{
  // four-sided dices ?
  unsigned int diceSize = 4;
  std::cin >> diceSize;
  // print the 3 most frequent fields
  unsigned int showFields = 3;
  std::cin >> showFields;

  // number of simulated moves
  const unsigned int Rolls = 1000000;//50000000;

  // always the same random numbers
  srand(0);

  // board setup
  const unsigned int NumFields = 40;
  // special field positions
  const unsigned int Go        =  0;
  const unsigned int Jail      = 10;
  const unsigned int GoToJail  = 30;
  const unsigned int Community  [3] = {  2, 17, 33 };
  const unsigned int Chance     [3] = {  7, 22, 36 };
  const unsigned int NextRailway[3] = { 15, 25,  5 }; // index x corresponds to Chance[x]
  const unsigned int NextUtility[3] = { 12, 28, 12 }; // index x corresponds to Chance[x]

  // random setup of community and chance cards, give them IDs 0..15
  std::vector<unsigned int> chance, community;
  for (unsigned int i = 0; i < 16; i++)
  {
    chance   .push_back(i);
    community.push_back(i);
  }
  std::random_shuffle(chance.begin(),    chance.end());
  std::random_shuffle(community.begin(), community.end());

  // Monte-Carlo simulation
  unsigned int current = Go;
  unsigned int doubles = 0;
  std::vector<unsigned long long> count(NumFields, 0);
  for (unsigned int rolls = 0; rolls < Rolls; rolls++)
  {
    // roll the dice ...
    unsigned int dice1 = (rand() % diceSize) + 1;
    unsigned int dice2 = (rand() % diceSize) + 1;
    // next field
    unsigned int next = (current + dice1 + dice2) % NumFields;

    // double ?
    if (dice1 == dice2)
      doubles++;
    else
      doubles = 0;

    if (doubles == 3)
    {
      next = Jail;
      doubles = 0;
    }

    // chance
    if (next == Chance[0] || next == Chance[1] || next == Chance[2])
    {
      // ID of current chance field (needed to figure out next railway / utility)
      int id = 0;
      if (next == Chance[1])
        id = 1;
      if (next == Chance[2])
        id = 2;

      // check card ID of the topmost card
      switch (chance.front())
      {
      case 0: next = Go; break;
      case 1: next = Jail; break;
      case 2: next = 11; break; // C1
      case 3: next = 24; break; // E3
      case 4: next = 39; break; // H2
      case 5: next =  5; break; // R1
              // go back 3 squares (might land on GoToJail, too)
              // awkward formula needs to take care of CC1 which redirects to H2
      case 6: next = (next + NumFields - 3) % NumFields; break;
      case 7: // two railway cards
      case 8: next = NextRailway[id]; break;
      case 9: next = NextUtility[id]; break;
      default: break;
      }

      // move card to the bottom
      std::rotate(chance.begin(), chance.begin() + 1, chance.end());
    }

    // community
    if (next == Community[0] || next == Community[1] || next == Community[2])
    {
      // check card ID of the topmost card
      switch (community.front())
      {
      case 0: next = Go;   break;
      case 1: next = Jail; break;
      default: break;
      }

      // move card to the bottom
      std::rotate(community.begin(), community.begin() + 1, community.end());
    }

    if (next == GoToJail)
      next = Jail;

    count[next]++;
    current = next;
  }

  // normalize (actually not needed, just to ease debugging)
  unsigned long long sum = 0;
  for (auto x : count)
    sum += x;

  // use std::map's implicit sorting
  std::multimap<double, unsigned int> sorted; // multimap in case of identical probabilities
  for (unsigned int i = 0; i < count.size(); i++)
    sorted.insert(std::make_pair(count[i] * 100.0 / sum, i));

  // extract the field positions
  std::vector<unsigned int> result;
  for (auto x : sorted)
    result.push_back(x.second);

  // and show the most frequent ones (they are at the end of the container)
  auto i = result.rbegin();

#define ORIGINAL
#ifdef ORIGINAL

  for (unsigned int j = 0; j < showFields; j++)
    std::cout << std::setw(2) << std::setfill('0') << *i++;

#else

  // field names
  const char* names[] = { "GO",   "A1", "CC1", "A2",  "T1", "R1", "B1",  "CH1", "B2", "B3",
                          "JAIL", "C1", "U1",  "C2",  "C3", "R2", "D1",  "CC2", "D2", "D3",
                          "FP",   "E1", "CH2", "E2",  "E3", "R3", "F1",  "F2",  "U2", "F3",
                          "G2J",  "G1", "G2",  "CC3", "G3", "R4", "CH3", "H1",  "T2", "H2" };
  for (unsigned int j = 0; j < showFields; j++)
    std::cout << names[*i++] << " ";

#endif

  return 0;
}
