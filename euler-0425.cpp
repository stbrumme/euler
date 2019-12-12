// ////////////////////////////////////////////////////////
// # Title
// Prime connection
//
// # URL
// https://projecteuler.net/problem=425
// http://euler.stephan-brumme.com/425/
//
// # Problem
// Two positive numbers `A` and `B` are said to be connected (denoted by `A \leftrightarrow B`) if one of these conditions holds:
// (1) `A` and `B` have the same length and differ in exactly one digit; for example, `123 \leftrightarrow 173`.
// (2) Adding one digit to the left of `A` (or `B`) makes `B` (or `A`); for example, `23 \leftrightarrow 223` and `123 \leftrightarrow 23`.
//
// We call a prime `P` a 2's relative if there exists a chain of connected primes between 2 and `P` and no prime in the chain exceeds `P`.
//
// For example, 127 is a 2's relative. One of the possible chains is shown below:
// `2 \leftrightarrow 3 \leftrightarrow 13 \leftrightarrow 113 \leftrightarrow 103 \leftrightarrow 107 \leftrightarrow 127`
// However, 11 and 103 are not 2's relatives.
//
// Let `F(N)` be the sum of the primes `<= N` which are not 2's relatives.
// We can verify that `F(10^3) = 431` and `F(10^4) = 78728`.
//
// Find `F(10^7)`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// My solution consists of four parts:
// 1. run a prime sieve (up to `10^7`)
// 2. find which primes are directly connected to each prime
// 3. find the paths between 2 and every prime where the highest number along the path is minimized
// 4. count all number that are either not connected to 2 or where the highest number along the path exceed that prime
//
// Step 1 can be solved easily by copying my standard prime sieve from my [toolbox](/toolbox/).
//
// Step 2 is encapsulated in the ''findEdges'' function. It iterates over all prime numbers and increments each digit until it reaches 9.
// There is no need to decrement a digit because those were already processes in previous iterations.
// For example, 103 is connected to 113. When processed 103, it will encounter 113 and add that connection ''103 ==> 113'' as well ''113 ==> 103'' in ''connected''.
// Then 113 only has to look at 123, 133, 143, ... (as well as playing around with the first and last digit).
// Prepending a digit is the same as pretending that the current number has a leading zero and treating it like any other digit.
// Note: initially I worked with ''std::string''s instead of my current "numbers only" approach but that was obviously very slow.
//
// Step 3 is performed by ''findLowestPaths'': beginning with 2 I trace the paths to all prime numbers.
// A priority queue is initially filled with 2 only and then all its connected primes are added.
// Each iteration picks the lowest number from the priority queue and adds its connected prime numbers if their path to 2 either
// wasn't observed so far or is optimized in the current iteration.
//
// The final step adds those numbers that don't exist in ''best'' or where ''best[x] > x'', that means the best path to 2 contains a higher number.

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <functional>

// ---------- standard prime sieve from my toolbox

// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;

// return true, if x is a prime number
bool isPrime(unsigned int x)
{
  // handle even numbers
  if ((x & 1) == 0)
    return x == 2;

  // lookup for odd numbers
  return sieve[x >> 1];
}

// find all prime numbers from 2 to size
void fillSieve(unsigned int size)
{
  // store only odd numbers
  const unsigned int half = size >> 1;

  // allocate memory
  sieve.resize(half, true);
  // 1 is not a prime number
  sieve[0] = false;

  // process all relevant prime factors
  for (unsigned int i = 1; 2 * i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3 * i + 1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2 * i + 1;
      }
    }
}

// ---------- problem specific code ----------

typedef std::unordered_map<unsigned int, std::vector<unsigned int>> Edges;
// create graph
Edges findEdges(unsigned int limit)
{
  Edges connected;
  // pre-allocate some memory
  connected.reserve(limit / 10);

  for (unsigned int i = 2; i < limit; i++)
  {
    if (!isPrime(i))
      continue;

    const unsigned int MaxPos = 7;
    // split i into its digits but keep their ten's exponents
    // e.g. 1234 => split = { 4, 30, 200, 1000, 0, 0, 0 };
    unsigned int split[MaxPos] = { 0,0,0,0,0,0,0 };
    unsigned int shift = 1;
    auto reduced = i;
    for (unsigned int pos = 0; pos < MaxPos; pos++)
    {
      shift *= 10;
      split[pos] = reduced % shift;
      reduced   -= reduced % shift;
    }

    shift = 1;
    for (unsigned int pos = 0; shift < 10*i && shift < limit; pos++, shift *= 10)
    {
      auto current = i;

      // analyze all bigger numbers
      for (unsigned int digit = split[pos] + shift; digit <= 9 * shift; digit += shift)
      {
        current += shift;
        if (isPrime(current))
        {
          // add if not existing (A => B)
          if (std::find(connected[i].begin(), connected[i].end(), current) == connected[i].end())
            connected[i].push_back(current);

          // add if not existing (B => A)
          if (std::find(connected[current].begin(), connected[current].end(), i) == connected[current].end())
          {
            // pre-allocate some memory
            connected[current].reserve(8);
            connected[current].push_back(i);
          }
        }
      }
    }
  }

  return connected;
}

typedef std::unordered_map<unsigned int, unsigned int> Best;
// return the minimized highest number between 2 and every prime
Best findLowestPaths(const Edges& connected)
{
  // best[x] is the lowest number on the path between 2 and x
  Best best;

  std::priority_queue<unsigned int, std::vector<unsigned int>, std::greater<unsigned int> > todo;
  todo.push(2);

  while (!todo.empty())
  {
    auto current = todo.top();
    todo.pop();

    // highest number so far
    auto top = best[current];
    // include current number, too
    if (top < current)
      top = current;

    auto connections = connected.find(current);
    if (connections == connected.end())
      continue;

    for (auto edge : connections->second)
    {
      auto high = best[edge];
      // no path or a worse path ?
      if (high == 0 || top < high)
      {
        // update with best value so far
        best[edge] = top;

        // re-evaluate
        todo.push(edge);
      }
    }
  }

  return best;
}

int main()
{
  unsigned int limit = 10000000;
  std::cin >> limit;

  // generate enough primes
  fillSieve(limit);

  // create graph
  auto connected = findEdges(limit);

  // find best path from 2 to each prime
  auto best = findLowestPaths(connected);

  // count primes with a connection to 2
  // or a path containing too high numbers
  unsigned long long result = 0;
  for (unsigned int i = 3; i < limit; i += 2)
    if (isPrime(i) && (best[i] == 0 || best[i] > i))
      result += i;

  // that's it !
  std::cout << result << std::endl;
  return 0;
}
