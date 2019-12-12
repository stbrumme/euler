// ////////////////////////////////////////////////////////
// # Title
// Binary Circles
//
// # URL
// https://projecteuler.net/problem=265
// http://euler.stephan-brumme.com/265/
//
// # Problem
// `2^N` binary digits can be placed in a circle so that all the `N`-digit clockwise subsequences are distinct.
//
// For `N=3`, two such circular arrangements are possible, ignoring rotations:
//
// ![binary circles](p265_BinaryCircles.gif)
//
// For the first arrangement, the 3-digit subsequences, in clockwise order, are:
// 000, 001, 010, 101, 011, 111, 110 and 100.
//
// Each circular arrangement can be encoded as a number by concatenating the binary digits starting with the subsequence of all zeros
// as the most significant bits and proceeding clockwise. The two arrangements for `N=3` are thus represented as 23 and 29:
// `00010111_2 = 23`
// `00011101_2 = 29`
//
// Calling `S(N)` the sum of the unique numeric representations, we can see that `S(3) = 23 + 29 = 52`.
//
// Find `S(5)`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I wrote a straightforward backtracking solution that start with `N` bits which are all zero and calls ''search'' recursively:
// - try to append another zero, try to append another one
// - if the most recent `N` bits occurred in the sequence before, then go back
// - if the sequence has `2^N` bits, then another solution was found
//
// To simplify (read: to speed up) the search for "used" sub-sequences, a bitmask called ''history'' tracks all numbers.
// If its k-th bit is used, then the number k is already part of the sequence.

#include <iostream>

// N = number of bits
unsigned int bits = 5;

// find all valid results
unsigned long long search(unsigned long long history, unsigned long long sequence)
{
  // bitmask for all relevant bits
  const unsigned int Mask = (1 << bits) - 1;

  // seen all 2^bits bit combinations ?
  unsigned long long allBits = (1ULL << (1 << bits)) - 1;
  if (history == allBits)
  {
    // remove lowest bits (which are all zeros)
    sequence >>= bits - 1;
    return sequence;
  }

  // shift most recent bits one position to the left, remove highest bit
  unsigned int next = (sequence << 1) & Mask;

  // two options for the following value
  unsigned int zero = next;
  unsigned int one  = next + 1;

  // sum of all found sequences
  unsigned long long result = 0;

  // check "zero"
  if ((history & (1ULL << zero)) == 0)
    result += search(history | (1ULL << zero),  sequence << 1);

  // check "one"
  if ((history & (1ULL << one )) == 0)
    result += search(history | (1ULL << one ), (sequence << 1) | 1);

  return result;
}

int main()
{
  std::cin >> bits;
  std::cout << search(1, 0) << std::endl;
  return 0;
}
