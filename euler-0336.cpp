// ////////////////////////////////////////////////////////
// # Title
// Maximix Arrangements
//
// # URL
// https://projecteuler.net/problem=336
// http://euler.stephan-brumme.com/336/
//
// # Problem
// A train is used to transport four carriages in the order: ABCD.
// However, sometimes when the train arrives to collect the carriages they are not in the correct order.
// To rearrange the carriages they are all shunted on to a large rotating turntable.
// After the carriages are uncoupled at a specific point the train moves off the turntable pulling the carriages still attached with it.
// The remaining carriages are rotated 180 degrees. All of the carriages are then rejoined and this process is repeated
// as often as necessary in order to obtain the least number of uses of the turntable.
// Some arrangements, such as ADCB, can be solved easily: the carriages are separated between A and D, and after DCB are rotated the correct order has been achieved.
//
// However, Simple Simon, the train driver, is not known for his efficiency, so he always solves the problem by initially getting carriage A in the correct place,
// then carriage B, and so on.
//
// Using four carriages, the worst possible arrangements for Simon, which we shall call maximix arrangements, are DACB and DBAC; each requiring him five rotations
// (although, using the most efficient approach, they could be solved using just three rotations).
// The process he uses for DACB is shown below.
//
// ![train](p336_maximix.gif)
//
// It can be verified that there are 24 maximix arrangements for six carriages, of which the tenth lexicographic maximix arrangement is DFAECB.
//
// Find the 2011th lexicographic maximix arrangement for eleven carriages.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Simple Simon works as follows:
// 1. find the next carriage (first is 'A', then 'B', etc.)
// 2. rotate all carriages from that position to the end
// 3. rotate all carriages from the final position to the end
// 4. continue with next carriage in step 1
//
// STL's ''std::rotate'' and ''std::next_permutation'' are very efficient default implementations and used throughout my solution.
//
// The maximum number of rotation is `2(n-1) - 1` because the last carriage needs no rotation and the
// second-to-last needs only one rotation.
//
// A few performance tweaks:
// (a) if a carriage is already in its final position, then Simple Simon can proceed with the next carriage
// ==> saving two rotations - it can't be a maximix arrangement
// (b) if a carriage is already at the end of the train, then Simple Simon needs only one rotation
// ==> it can't be a maximix arrangement, except when he is about to rearrange the last two carriages, then it's ok
//
// These two "rules" lead to some conclusions:
// - a maximix arrangement can't start with an 'A' because that violates rule (a)
// - a maximix arrangement can't start with an 'B' because after the first rotation, 'A' will be the last carriage while 'B' is the first
// ==> after the second rotation 'A' will be the first and 'B' the last
// ==> only one rotation (the third) places 'B' in its correct slot (see rule (b) )
//
// Therefore my initial train configuration is "CABDEFGHIJKLM" (the lexigraphic smallest that neither starts with 'A' nor 'B').
//
// # Note
// There are tons of further performance optimizations but the solution is found in less than 0.1 seconds - fast enough for me.
//
// # Alternative
// I looks like you can solve this recursively with Dynamic Programming.

#include <iostream>
#include <string>
#include <algorithm>

int main()
{
  // can't start with an 'A' or 'B'
  std::string train = "CABDEFGHIJKLM"; // up to 14 for live test

  // look for the 2011th maximix train
  auto stopWhenFound = 2011;
  auto numFound = 0;

  // shorter train
  auto length = train.size();
  std::cin >> length >> stopWhenFound;
  train.erase(length);

  // two for each carriage, except for the last two which require just one rotation
  unsigned int maxRotations = (train.size() - 1) * 2 - 1;

  do
  {
    auto current = train;
    // move carriage 'A', then 'B', then 'C', ... to its correct position
    auto expect  = 'A';

    // count rotations
    unsigned int rotations = 0;
    for (size_t i = 0; i < current.size() - 1; i++, expect++)
    {
      // carriage already in correct position ?
      // => can't be a maximix arrangement
      if (current[i] == expect)
        break;
      // carriage in last position ? only allowed for 'J' (the second-to-last carriage)
      if (current[current.size() - 1] == expect && i != current.size() - 2)
        break;

      // find carriage
      auto j = i + 1;
      while (current[j] != expect)
        j++;

      // rotate carriage to the end of the train (unless it's already there)
      if (j < current.size() - 1)
      {
        std::reverse(current.begin() + j, current.end());
        rotations++;
      }

      // rotate carriage to its correct position
      std::reverse(current.begin() + i, current.end());
      rotations++;
    }

    // is it a maximix arrangement ?
    if (rotations == maxRotations)
    {
      numFound++;
      // found 2011th maximix ?
      if (numFound == stopWhenFound)
        break;
    }
  } while (std::next_permutation(train.begin(), train.end()));

  std::cout << train << std::endl;
  return 0;
}
