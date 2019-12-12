// ////////////////////////////////////////////////////////
// # Title
// Pandigital prime sets
//
// # URL
// https://projecteuler.net/problem=118
// http://euler.stephan-brumme.com/118/
//
// # Problem
// Using all of the digits 1 through 9 and concatenating them freely to form decimal integers, different sets can be formed.
// Interestingly with the set {2,5,47,89,631}, all of the elements belonging to it are prime.
//
// How many distinct sets containing each of the digits one through nine exactly once contain only prime elements?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// First, I create a prime sieve for all prime numbers up to 100000000.
// My ''isPrime'' can handle larger number, too, but has to revert to trial division (which is much, much slower).
//
// The core routine is ''search'' which takes a vector ''digits'' and looks at all digits starting at position ''firstPos''.
// It appends all digits step-by-step to a local variable ''current'' and checks whether it is prime.
// If yes, then the routine appends that prime number to ''merged''  and calls itself recursively.
// If there are no more digits left in ''digits'' (that means ''firstPos == digits.size()'') then the numbers in ''merged'' are a valid solution.
//
// It took me some time to figure out that all numbers in ''merged'' must be in ascending order to avoid finding the same solution multiple times.
//
// # Hackerrank
// You are given a certain set of digits and have to find the sum of all solutions.
// Update November 2017: Kevin McShane helped me to fix a bug in my "premature optimization" in line 114 - now the code solves all test cases.
//
// # Note
// 100,000,000 seems to be the "sweet spot" for my prime sieve. Higher values increase the time needed to fill the sieve significantly,
// while lower values lead to a slower ''isPrime'' (because it has to use trial division more often).

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define ORIGINAL

/////////////// prime sieve ///////////////
// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;
// return true, if x is a prime number
bool isPrime(unsigned int x)
{
  // handle even numbers
  if ((x & 1) == 0)
    return x == 2;

  // trial division for large numbers
  if (x >= sieve.size() * 2)
  {
    for (unsigned int i = 3; i*i <= x; i += 2)
      if (x % i == 0)
        return false;
    return true;
  }

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


/////////////// solution ///////////////
typedef std::vector<unsigned int> Digits;
std::vector<std::vector<unsigned int>> solutions;
void search(const Digits& digits, std::vector<unsigned int>& merged, size_t firstPos = 0)
{
  // no more digits left => found a solution
  if (firstPos == digits.size())
  {
    solutions.push_back(merged);
    return;
  }

  // process one more digit at a time
  unsigned int current = 0;
  while (firstPos < digits.size())
  {
    // next digit
    current *= 10;
    current += digits[firstPos++];

    // must be larger than its predecessor
    if (!merged.empty() && current < merged.back())
      continue;

    // ... and prime, of course !
    if (isPrime(current))
    {
      merged.push_back(current);
      search(digits, merged, firstPos);
      merged.pop_back();
    }
  }
}

int main()
{
  // precompute primes (bigger primes are tested using trial division)
  fillSieve(100000000);

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    // read digits
    std::string strDigits = "123456789";
    std::cin >> strDigits;

    // convert to a sorted array/vector
    Digits digits;
    for (auto x : strDigits)
      digits.push_back(x - '0');
    std::sort(digits.begin(), digits.end());

    // discard solutions from previous tests
    solutions.clear();
    do
    {
      // simple speed optimization: last digit must be odd (or it's 2)
      if (digits.back() % 2 == 0 && (digits.size() > 1 || digits.back() != 2))
        continue;

      // let's go !
      std::vector<unsigned int> merged;
      search(digits, merged);
    } while (std::next_permutation(digits.begin(), digits.end()));

#ifdef ORIGINAL
    std::cout << solutions.size() << std::endl;
#else
    // compute sum of each solution
    std::vector<unsigned long long> sorted;
    for (auto merged : solutions)
    {
      unsigned long long sum = 0;
      for (auto x : merged)
        sum += x;
      sorted.push_back(sum);
    }
    // sort ascendingly
    std::sort(sorted.begin(), sorted.end());

    // remove duplicates (needed ?)
    //auto garbage = std::unique(sorted.begin(), sorted.end());
    //sorted.erase(garbage, sorted.end());

    // and print all of them
    for (auto x : sorted)
      std::cout << x << std::endl;
    std::cout << std::endl;
#endif
  }

  return 0;
}
