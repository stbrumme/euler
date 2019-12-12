// ////////////////////////////////////////////////////////
// # Title
// Hexagonal tile differences
//
// # URL
// https://projecteuler.net/problem=128
// http://euler.stephan-brumme.com/128/
//
// # Problem
// A hexagonal tile with number 1 is surrounded by a ring of six hexagonal tiles, starting at "12 o'clock" and numbering the tiles 2 to 7 in an anti-clockwise direction.
//
// New rings are added in the same fashion, with the next rings being numbered 8 to 19, 20 to 37, 38 to 61, and so on. The diagram below shows the first three rings.
//
// ![hexagon](p128.gif)
//
// By finding the difference between tile n and each of its six neighbours we shall define PD(n) to be the number of those differences which are prime.
// For example, working clockwise around tile 8 the differences are 12, 29, 11, 6, 1, and 13. So PD(8) = 3.
//
// In the same way, the differences around tile 17 are 1, 17, 16, 1, 11, and 10, hence PD(17) = 2.
// It can be shown that the maximum value of PD(n) is 3.
// If all of the tiles for which PD(n) = 3 are listed in ascending order to form a sequence, the 10th tile would be 271.
//
// Find the 2000th tile in this sequence.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// Tile 1 matches all conditions: PD(1) = 3 because 4-1=3 is prime,  3-1=2  is prime and 6-1=5 is prime.
// Tile 2 matches all conditions: PD(2) = 3 because 9-2=7 is prime, 19-2=17 is prime and 7-2=5 is prime.
//
// My first program performed that task for all numbers from 1 to 1000. It didn't find the solution because surprisingly only a tiny fraction of tiles matches all conditions.
//
// Then I looked at tile 17 more closely:
// - its predecessor and its successor are neighbors, their difference is 1 which is not a prime
// - on its left side (the ring inside) there are the two consecutive numbers 6 and 7. Only one difference is odd, the other is even (can't be a prime)
// - on its right side (the ring outside) there are the two consecutive numbers 33 and 34. Only one difference is odd, the other is even (can't be a prime)
// Without computing the differences 17-6, 17-7, 33-17 and 34-17 I know that at most two difference can yield a prime number.
// In general, every number located on the edge/side of a ring has PD(x) <= 2. Therefore 9, 11, 13, 15, 17, 21, 22, 24, 25, 27, 28, ... can be excluded.
// Note that there is a single exception: the last tiles of each ring (7, 19, 37, ...) have no connection to their successors. They remain candidates for PD(x) = 3.
//
// Now only the numbers on the "corners" of each ring are left (such as 2,3,4,5,6,7,8,10,12,14,16,18,20,23,26,...).
// Except for the corners "north" of 1 (2,8,20,..., they are the first numbers of a ring), each of those tiles is connected to its predecessor and its successor (remember: difference is 1 ==> no prime).
// The neighbors on the outside ring are three consecutive numbers. Only if the difference to the first and the last of these three tiles is odd, then at most two differences can be prime.
// For PD(x)=3, the inner tile must be prime, too. That means either the two relevant outer and the inner tile are odd while the corner tile is even or
// the two relevant outer and the inner tile are even while the corner tile is odd.
// The inner tile is a corner tile, too, and has the difference `6 * ring + corner` (where `1 <= corner <= 5`, I look at the "north" tile later).
// Clearly, the second and fourth corner have always an even difference and their PD(x) must be smaller than 3.
// The middle of the outer three tiles is a corner tile as well and has a difference of `6 * (ring+1) + corner`.
// Obviously, its predecessor and successor have a difference of `6 * (ring+1) + corner \pm 1`.
// The first, third and fifth corner produce an even difference which is never prime.
//
// These formulas are a bit off for the first 7 tiles because my formulas `6 * ring` and `6 * (ring+1)` are valid everywhere - but not in the centre.
// I manually inspected the first seven tiles and found that only tiles 1 and 2 match all conditions.
//
// The only tiles left are the first and last tiles of each ring. My program checks their surrounding tiles, or better say, their differences.
// The difference between the first tile of the current ring and the first tile of the previous ring is ''incrementFrom = (ring - 1) * 6'' (where ''ring'' denotes the current ring).
// The difference between the first tile of the current ring and the first tile of the next ring is ''incrementTo = ring * 6''.
// And the difference between the first tile of the current ring and the ring beyond the next ring is ''incrementTo2 = (ring + 1) * 6 + incrementTo''.
//
// There are six tiles around the first tile of ring and their differences are (beginning from the bottom at 6 o'clock, 8 o'clock, 10 o'clock, ..., 4 o'clock):
// ''a = incrementFrom'' ==> multiple of 6, never prime
// ''b = 1'' ==> never prime
// ''c = incrementTo + 1''
// ''d = incrementTo'' ==> multiple of 6, never prime
// ''e = incrementTo2 - 1''
// ''f = incrementTo - 1''
// Only three tiles are actual candidates (''c'', ''e'' and ''f'').
//
// There are six tiles around the last tile of ring and their differences are (same order as first tile):
// ''a = incrementTo'' ==> multiple of 6, never prime
// ''b = incrementFrom + incrementTo - 1''
// ''c = incrementTo - 1''
// ''d = incrementTo2 - incrementTo'' ==> multiple of 6, never prime
// ''e = incrementTo2 - incrementTo - 1''
// ''f = 1'' ==> never prime
// Only three tiles are actual candidates (''b'', ''c'' and ''e'').
//
// One difference appears for both tiles (''incrementTo - 1''). My program checks this difference first for a small performance gain.
//
// # Note
// The upper limit of my prime sieve was determined by running the program a few times and counting which primes where checked
// (by the way: I used the Miller-Rabin primality test but it is magnitudes slower than the current primitive sieve).

#include <iostream>
#include <vector>

// standard prime sieve, see my [toolbox](../toolbox/)
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

int main()
{
  // fill a prime sieve (maximum prime was determined by running it a few times)
#define ORIGINAL
#ifdef ORIGINAL
  unsigned int limit =  2000;
  fillSieve(835000);
#else
  unsigned int limit = 80000;
  fillSieve(91200000);
#endif

  // [nth number] => [tile]
  std::vector<unsigned long long> found = { 1, 2 };

  // tiles 1 and 2 are the first two tiles because PD(1)=PD(2)=3
  unsigned int numFound = 2;
  // and there are no further tiles below 8 where PD(x)=3
  unsigned long long first = 8;

  // analyze rings until enough numbers are found
  for (unsigned long long ring = 2; numFound < limit; ring++)
  {
    // difference to previous ring (inside current ring)
    auto incrementFrom = (ring - 1) * 6;
    // difference to next ring (outside current ring)
    auto incrementTo   =  ring      * 6;
    // difference to ring after the next ring
    auto incrementTo2  = (ring + 1) * 6 + incrementTo;

    // difference "incrementTo - 1" appears twice: around first and last tile
    if (!isPrime(incrementTo - 1))
    {
      first += incrementTo;
      continue;
    }

    // check first number of a ring
    if (isPrime(incrementTo + 1) && isPrime(incrementTo2 - 1))
    {
      numFound++;
      found.push_back(first);
    }

    // check last number of a ring
    if (isPrime(incrementFrom + incrementTo - 1) && isPrime(incrementTo2 - incrementTo - 1))
    {
      numFound++;

      auto last = first + incrementTo - 1;
      found.push_back(last);
    }

    // next ring
    first += incrementTo;
  }

  // look up results
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int index = 2000;
    std::cin >> index;
    std::cout << found[index - 1] << std::endl;
  }

  return 0;
}
