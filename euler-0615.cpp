// ////////////////////////////////////////////////////////
// # Title
// The millionth number with at least one million prime factors
//
// # URL
// https://projecteuler.net/problem=615
// http://euler.stephan-brumme.com/615/
//
// # Problem
// Consider the natural numbers having at least 5 prime factors, which don't have to be distinct.
// Sorting these numbers by size gives a list which starts with:
//
//     `32 = 2 * 2 * 2 * 2 * 2`
//     `48 = 2 * 2 * 2 * 2 * 3`
//     `64 = 2 * 2 * 2 * 2 * 2 * 2`
//     `72 = 2 * 2 * 2 * 3 * 3`
//     `80 = 2 * 2 * 2 * 2 * 5`
//     `96 = 2 * 2 * 2 * 2 * 2 * 3`
//      ...
//
// So, for example, the fifth number with at least 5 prime factors is 80.
//
// Find the millionth number with at least one million prime factors.
// Give your answer modulo 123454321.
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// Based on the example I figured out pretty soon that most of the "one million prime factors" will be 2s.
// The one millionth prime number is [15485863](http://www.wolframalpha.com/input/?i=1000000th+prime), therefore the smallest number with at least one million prime factors cannot exceed `2^999999 * 15485863` (see ''tooLarge'').
//
// I assumed that all but the last 32 prime factors will be 2s (in fact I later found empirically that all but the last 27 must be 2s).
// A minimum-priority-queue, that means smallest numbers first, is initialized with `2^32` (later changed to `2^27`).
// Then I pick and remove the smallest number ''current'' one million times and:
// - "append a prime":  multiply ''current''                 by `p` where `p` will be any of the first million primes - but this time I take care that `p` is no smaller than the largest prime factor of ''current''
// - "replace a prime": replace one of the 2s of ''current'' by `p` where `p` will be any of the first million primes - but this time I take care that `p` is no smaller than the largest prime factor of ''current'' (same condition as above)
// - if the new numbers exceed the limit mentioned above, then I continue with the next number
//
// After one million iterations the priority queue's ''top()'' element will be the smallest number with at least 32 (later reduced to 27) prime factors.
//
// Obviously I can't multiply that number by `2^{1000000-32}` with C++ native data types.
// However, only the residue modulo 123454321 is asked for:
// `(a * b) mod m = ((a mod m) * (b mod m)) mod m`
//
// Multiplying that number 999968 times by 2 while repeatedly applying modulo 123454321 seems to be pretty slow
// but GCC does an amazing job at optimizing this loop and it finished in a few milliseconds.
//
// # Note
// The code was slightly modified after I submitted the correct result: initially, I had an ''std::set<Number> candidates'' which spent most of the time allocating (and freeing) memory.
// The ''std::priority_queue'' is about twice as fast when its internal container is an ''std::vector''.
//
// Most of the one million primes will never be used so I looked for the smallest prime that still produces the correct result:
// Starting with 173207, which is the 15770th prime, instead of 15485863 brings down the execution time to about one third of a second (see ''#define FAST'').
// I found that number solely by trial'n'error.
//
// There is probably a smarter way to avoid the duplicates in ''candidates'' but I wasn't willing to spend much time on it.

#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <functional>
#include <set>

// ---------- a few routines from my toolbox ----------

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
  for (unsigned int i = 1; 2*i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3*i+1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2*i+1;
      }
    }
}

// ---------- problem-specific code ----------

// a tiny wrapper for a number and its largest prime factor
struct Number
{
  unsigned long long value;   // a number
  unsigned int largestFactor; // and its largest prime factor

  // for std::greater / min-queue
  bool operator>(const Number& other) const
  {
    return value > other.value;
  }
};

int main()
{
  unsigned int numCandidates = 1000000;
  std::cin >> numCandidates;

  const unsigned int Modulo  = 123454321;

  // generate the first million prime numbers
  unsigned int maxPrime = 15485863; // http://www.wolframalpha.com/input/?i=1000000th+prime
#define FAST
#ifdef  FAST
  if (numCandidates <= 1000000)
    maxPrime = 173207; // empirically determined limit
#endif

  // compute enough prime numbers
  fillSieve(maxPrime + 1);

  // copy prime numbers to a consecutive array (just for faster access)
  std::vector<unsigned int> primes = { 2 };
  primes.reserve(numCandidates);
  for (unsigned int i = 3; i <= maxPrime; i += 2)
    if (isPrime(i))
      primes.push_back(i);

  // empirically determined
  unsigned int numVariableFactors = 27;
  if (numCandidates < 27)
    numVariableFactors = numCandidates; // for live-test
  unsigned long long seed = 1 << numVariableFactors;

  // create one million numbers with exactly 27 prime factors of the form prime*2^26
  std::priority_queue<Number, std::vector<Number>, std::greater<Number>> candidates;
  candidates.push({ seed, 2 });

  // currently 1000000th number (there are a few smaller numbers that need to be discovered now)
  auto tooLarge = seed / 2 * primes.back(); // largest number in the candidates container

  // detect duplicates
  unsigned long long previous = 0;
  // look at the candidates in ascending order
  for (unsigned int numProcessed = 0; numProcessed < numCandidates; numProcessed++)
  {
    // pick next number
    auto i = candidates.top();
    auto current = i.value;
    candidates.pop();

    // skip duplicates
    while (current == previous)
    {
      i = candidates.top();
      current = i.value;
      candidates.pop();
    }
    auto maxFactor = i.largestFactor;
    previous = current;

    // "append" a prime to the current candidate
    for (auto p : primes)
    {
      auto next = current * p;
      if (next >= tooLarge)
        break;
      if (p >= maxFactor)
        candidates.push({ next, p });
    }

    // replace a 2 by a prime (pretty much the same loop, only "next" is computed slightly different)
    for (auto p : primes)
    {
      auto next = (current / 2) * p;
      if (next >= tooLarge)
        break;
      if (p >= maxFactor && p > 2) // no use in replacing 2 by 2
        candidates.push({ next, p });
    }
  }

  // pick the 1000000th number
  unsigned int result = candidates.top().value % Modulo;
  // multiply by 2^1000000 (except for the 27x 2's "used")
  for (unsigned int i = numVariableFactors; i < numCandidates; i++)
    result = (result * 2) % Modulo; // I thought that simple loop would be slow (because of "expensive" modulo),
                                    // but GCC converts it to impressively fast code !

  std::cout << result << std::endl;
  return 0;
}
