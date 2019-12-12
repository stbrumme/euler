// ////////////////////////////////////////////////////////
// # Title
// Nim
//
// # URL
// https://projecteuler.net/problem=301
// http://euler.stephan-brumme.com/301/
//
// # Problem
// Nim is a game played with heaps of stones, where two players take it in turn to remove any number of stones from any heap until no stones remain.
//
// We'll consider the three-heap normal-play version of Nim, which works as follows:
// - At the start of the game there are three heaps of stones.
// - On his turn the player removes any positive number of stones from any single heap.
// - The first player unable to move (because no stones remain) loses.
//
// If (n1,n2,n3) indicates a Nim position consisting of heaps of size n1, n2 and n3 then there is a simple function X(n1,n2,n3)
// that you may look up or attempt to deduce for yourself that returns:
//
// - zero if, with perfect strategy, the player about to move will eventually lose; or
// - non-zero if, with perfect strategy, the player about to move will eventually win.
//
// For example X(1,2,3) = 0 because, no matter what the current player does, his opponent can respond with a move that leaves two heaps of equal size, at which point every move by the current player can be mirrored by his opponent until no stones remain; so the current player loses. To illustrate:
// - current player moves to (1,2,1)
// - opponent moves to (1,0,1)
// - current player moves to (0,0,1)
// - opponent moves to (0,0,0), and so wins.
//
// For how many positive integers `n <= 2^30` does X(n,2n,3n) = 0 ?
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// This Wikipedia page told me that the "Nim Sum" is the XOR-result of all heaps: https://en.wikipedia.org/wiki/Nim
//
// # Note
// I reversed the loop to iterate from `10^30` to `0` because then my compiler can produce code that is about 10% faster.
//
// # Alternative
// Often I enter a few results into my search engine of choice - this time I was surprised to see that
// the results for `2^1`, `2^2`, `2^3`, ... `2^n` are the (n+2)-th Fibonacci numbers.
// Someone with more mathematical insight could probably seen that before writing the code - but I can't.
//
// It's possible to write a Dynamic Programming solution, too:
// when n contains no two consecutive set bits then the game is lost.

#include <iostream>

// see https://en.wikipedia.org/wiki/Nim
unsigned int evaluate(unsigned int n1, unsigned int n2, unsigned int n3)
{
  return n1 ^ n2 ^ n3;
}

int main()
{
  // search up to n = 2^exponent
  unsigned int exponent = 30;
  std::cin >> exponent;

  unsigned int lost = 0;
  // 2^30 => about 1 billion values
  for (unsigned int n = 1U << exponent; n > 0; n--)
  {
    auto score = evaluate(n, 2*n, 3*n);
    if (score == 0)
      lost++;
  }

  std::cout << lost << std::endl;
  return 0;
}
