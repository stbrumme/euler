// ////////////////////////////////////////////////////////
// # Title
// Skew-cost coding
//
// # URL
// https://projecteuler.net/problem=219
// http://euler.stephan-brumme.com/219/
//
// # Problem
// Let A and B be bit strings (sequences of 0's and 1's).
// If A is equal to the leftmost length(A) bits of B, then A is said to be a prefix of B.
// For example, 00110 is a prefix of 001101001, but not of 00111 or 100110.
//
// A prefix-free code of size n is a collection of n distinct bit strings such that no string is a prefix of any other.
// For example, this is a prefix-free code of size 6:
// 0000, 0001, 001, 01, 10, 11
//
// Now suppose that it costs one penny to transmit a '0' bit, but four pence to transmit a '1'.
// Then the total cost of the prefix-free code shown above is 35 pence, which happens to be the cheapest possible for the skewed pricing scheme in question.
// In short, we write `Cost(6) = 35`.
//
// What is `Cost(10^9)` ?
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// The basic idea behind the construction of prefix-free codes is outlined on the Wikipedia page about Huffman codes (see https://en.wikipedia.org/wiki/Huffman_coding ):
// - add all codes to a priority-queue sorted by their weight
// - pick the code from the queue's front and create two need codes: append a 0 and a 1 and insert those two codes in the priority queue
//
// Initially there are two codes: 0 (weight 1) and 1 (weight 4).
// The algorithm then has to find the remaining `10^9 - 2` codes and keep track of their cost.
//
// Even though the correct result is found, this algorithm is pretty slow (''queue'' needs 147 seconds).
// When I looked at the lengths of the codes I saw that they are pretty short. That means that their cost is pretty low, too.
// ''queue'' was repeatedly picking codes with the same cost from its storage. And adding children to the same two categories: plus 1 and plus 4 pence.
//
// That's why I wrote a different approach called ''array'':
// Don't keep track of every single code - just count how many codes with a certain weight exists.
// Initially there is one code with weight 1 and one code with weight 4.
// Then the algorithm is as follows:
// - pick all codes with the lowest weight from ''costs[x]''
// - append a zero and a one in order to create their children: ''costs[x + 1] += costs[x]'' and ''costs[x + 4] += costs[x]''
//
// # Note
// The STL's ''priority_queue'' is a max-heap, that means that ''top()'' always returns the largest element.
// However, I need the smallest element for my program and therefore needed to use ''std::greater'' for comparisons.
// The peak memory usage of my first algorithm was about 1 GByte.

#include <iostream>
#include <queue>
#include <vector>
#include <functional>

// bit sequence
typedef unsigned char Cost;

// find result using a priority_queue
unsigned long long queue(unsigned int limit)
{
  std::priority_queue<Cost, std::vector<Cost>, std::greater<Cost>> codes; // min-heap instead of default max-heap
  // first two codes
  codes.push(1);
  codes.push(4);
  unsigned long long totalCost = 5; // sum of the first two codes: 1+4

  // until enough codes generated
  unsigned int numCodes = 2;
  while (numCodes < limit)
  {
    // pick the first
    auto current = codes.top();
    codes.pop();
    // add two new children codes
    codes.push(current + 1);
    codes.push(current + 4);

    // keep track of the cost
    numCodes++;
    totalCost += current + 1 + current + 4 - current; // same as current + 5
  }

  return totalCost;
}

// find result using a bit-length counters
unsigned long long array(unsigned int limit)
{
  std::vector<unsigned long long> costs(70, 0);
  // initial single-bit codes: "0" => weight 1, "1" => weight 4
  costs[1] = 1;
  costs[4] = 1;
  unsigned long long totalCost = 1 + 4;

  // start with the lowest weight (which is 1)
  auto current = 1;
  // number of codes that I need to generate
  auto remaining = limit - 2;
  while (remaining > 0)
  {
    // all codes "used" of the current weight ? => look at higher weights
    while (costs[current] == 0) // no gaps: "if" instead of "while" works as well
      current++;

    // try to process all codes of a certain weight at once
    auto block = costs[current];
    // except when I don't need all of them
    if (block > remaining)
      block = remaining;

    // adjust counters
    remaining          -= block;
    costs[current]     -= block;
    costs[current + 1] += block;
    costs[current + 4] += block;
    // weight is block * (current + 1 + current + 4 - current)
    totalCost          += block * (unsigned long long)(current + 5);
  }

  return totalCost;
}

int main()
{
  unsigned int limit = 1000000000;
  std::cin >> limit;

  // slow algorithm
  //std::cout << queue(limit) << std::endl;

  // fast algorithm
  std::cout << array(limit) << std::endl;

  return 0;
}
