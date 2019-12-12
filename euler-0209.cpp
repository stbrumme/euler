// ////////////////////////////////////////////////////////
// # Title
// Circular Logic
//
// # URL
// https://projecteuler.net/problem=209
// http://euler.stephan-brumme.com/209/
//
// # Problem
// A k-input binary truth table is a map from k input bits (binary digits, 0 [false] or 1 [true]) to 1 output bit.
// For example, the 2-input binary truth tables for the logical AND and XOR functions are:
//
// ||  2 ||  2 ||    5    ||
// ||! x ++  y ++ x AND y ||
// ||  0 ++  0 ++    0    ||
// ||  0 ++  1 ++    0    ||
// ||  1 ++  0 ++    0    ||
// ||  1 ++  1 ++    1    ||
//
// ||  2 ||  2 ||    5    ||
// ||! x ++  y ++ x XOR y ||
// ||  0 ++  0 ++    0    ||
// ||  0 ++  1 ++    1    ||
// ||  1 ++  0 ++    1    ||
// ||  1 ++  1 ++    0    ||
//
// How many 6-input binary truth tables, `tau`, satisfy the formula
// `tau(a, b, c, d, e, f) \space \text{AND} \space tau(b, c, d, e, f, a \space \text{XOR} \space (b \space \text{AND} \space c)) = 0`
// for all 6-bit inputs `(a, b, c, d, e, f)`?
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// The most crucial step is to recognize that there is a 1:1 mapping (bijective mapping) between `(a, b, c, d, e, f)` and `(b, c, d, e, f, a \space \text{XOR} \space (b \space \text{AND} \space c)) = 0`.
// Therefore my program maps each possible input `(a, b, c, d, e, f)` to `(b, c, d, e, f, a \space \text{XOR} \space (b \space \text{AND} \space c)) = 0`.
// These six variables are equivalent to a set of all binary numbers from `0` to `2^6 - 1` (`=63`).
// I store that mapping in ''connections''.
//
// Then I have to find all cycles: how to long it take that a number is mapped to itself: ''connections[connections[connections[...connections[x]]]] = x''.
// Each number is part of exactly one cycle - most are very short but the longest has 46 elements.
// Note that if ''x'' maps to itself after ''s'' steps, then each other number of the same cycle maps to itself after ''s'' steps, too.
// Therefore all numbers in the same cycle can be treated equally.
//
// I computed the number of possibilities for small cycle lengths (essentially they can't can have consecutive ones) and looked up my values in OEIS.
// They are called Lucas numbers (see https://en.wikipedia.org/wiki/Lucas_number ) and extremely similar to the Fibonacci sequence.
//
// Each cycle is independent of each other therefore the Lucas numbers of all cycles have to be multiplied.
//
// # Note
// There is no live test available for my solution.

#include <iostream>
#include <bitset> // I often prefer std::vector<bool> but let's try some of the rarely included C++ containers !

int main()
{
  // 2^6 = 64
  const auto SixtyFour = 64;

  // go through all 2^6 = 64 possible inputs of tau(a,b,c,d,e,f) and find the corresponding state tau(b,c,d,e,f,a XOR (b AND c))
  unsigned char connections[SixtyFour];
  for (auto from = 0; from < SixtyFour; from++) // state of first tau
  {
    std::bitset<6> leftSide = from;
    std::bitset<6> rightSide;

    rightSide[0] = leftSide[1]; // b => a
    rightSide[1] = leftSide[2]; // c => b
    rightSide[2] = leftSide[3]; // d => c
    rightSide[3] = leftSide[4]; // e => d
    rightSide[4] = leftSide[5]; // f => e
    rightSide[5] = leftSide[0] ^ (leftSide[1] & leftSide[2]); // a ^ (b & c) => f

    // connections[from] = to
    connections[from] = rightSide.to_ulong();
  }

  // precompute Lucas numbers
  unsigned long long lucas[SixtyFour + 1] = { 2, 1 }; // seeds
  for (auto i = 2; i <= SixtyFour; i++)     // actually I don't need all of them, longest cycle is < 64
    lucas[i] = lucas[i - 2] + lucas[i - 1]; // computation is similar to Fibonacci, but different seeds

  // multiply Lucas numbers of each cycle length
  unsigned long long result = 1;

  // find cycles
  std::bitset<SixtyFour> used = 0; // set used[x] to true if state x was processed
  while (!used.all())
  {
    // pick a randomly chosen available state
    // I always take the smallest / lowest but it doesn't really matter which one I pick
    auto start = 0;
    while (used[start])
      start++;

    // walk through the states until the initial state is reached again
    auto current = start;
    auto cycleLength = 0;
    do
    {
      // "use" this state
      used[current] = true;
      cycleLength++;

      // continue with next state in this cycle
      current = connections[current];
    } while (current != start);

    // include all those combinations
    result *= lucas[cycleLength];
  }

  // hooray ...
  std::cout << result << std::endl;
  return 0;
}
