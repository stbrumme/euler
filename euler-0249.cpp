// ////////////////////////////////////////////////////////
// # Title
// Prime Subset Sums
//
// # URL
// https://projecteuler.net/problem=249
// http://euler.stephan-brumme.com/249/
//
// # Problem
// Let `S = {2, 3, 5, ..., 4999}` be the set of prime numbers less than 5000.
//
// Find the number of subsets of `S`, the sum of whose elements is a prime number.
// Enter the rightmost 16 digits as your answer.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// The efficient prime sieve was copied from my [toolbox](/toolbox/).
//
// Initially, there is one empty set with sum zero.
// Then I iterate over all prime numbers up to 5000 and add each prime to each set.
// For example, 2 is added to the empty set so that its sum is 2.
// There was no set with sum=2 but the number of sets with sum=0 is added to number of sets with sum=2.
// Then 3 is added to each set: to the empty set with sum=0 and the set with sum=2.
// Hence there is now one more set with sum=0+3=3 and one with sum=2+3=5.
// When the next prime number is processed, it's 5, it's added to the one empty sets.
// There was already one set with sum=5, now there is a second one !
// (and at least one new set with sum=2+5=7, sum=3+5=8 and sum=5+5=10).
//
// To avoid overflows, the number of sets is repeatedly truncated to the last 16 digits (`mod 10^16`).
// I achieved a significant performance improvement by processing the sums in descending order because then
// I only need one array instead of two (otherwise collisions produce a wrong result).
//
// The last step is to count only those sums that are prime numbers.

#include <iostream>
#include <vector>

const auto Modulo = 10000000000000000ULL;

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

int main()
{
  unsigned int limit = 5000;
  std::cin >> limit;

  // sum of all prime numbers below 5000
  fillSieve(limit);
  unsigned int maxSum = 2;
  for (unsigned int i = 3; i <= limit; i += 2)
    if (isPrime(i))
      maxSum += i;

  // one empty set, its sum is zero
  std::vector<unsigned long long> count(maxSum + 1, 0);
  unsigned int largest = 0;
  count[largest] = 1;

  // add each prime to each set
  for (unsigned int i = 2; i < limit; i++)
    if (isPrime(i))
    {
      largest += i;
      // look at all potential sums of sets
      for (auto j = largest; j >= i; j--)
      {
        count[j] += count[j - i];
        count[j] %= Modulo;
      }
    }

  // enlarge prime sieve
  fillSieve(maxSum); // maxSum is about 1600000
  // sum of all count where index is a prime number
  unsigned long long result = 0;
  for (unsigned int i = 0; i < count.size(); i++)
    if (isPrime(i))
    {
      result += count[i];
      result %= Modulo;
    }

  // show result
  std::cout << result << std::endl;
  return 0;
}
