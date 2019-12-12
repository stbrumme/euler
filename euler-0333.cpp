// ////////////////////////////////////////////////////////
// # Title
// Special partitions
//
// # URL
// https://projecteuler.net/problem=333
// http://euler.stephan-brumme.com/333/
//
// # Problem
// All positive integers can be partitioned in such a way that each and every term of the partition can be expressed as `2^i * 3^j`, where `i,j >= 0`.
//
// Let's consider only those such partitions where none of the terms can divide any of the other terms.
// For example, the partition of `17 = 2 + 6 + 9 = 2^1 * 3^0 + 2^1 * 3^1 + 2^0 * 3^2` would not be valid since 2 can divide 6.
// Neither would the partition `17 = 16 + 1 = 2^4 * 3^0 + 2^0 * 3^0` since 1 can divide 16.
// The only valid partition of `17` would be `8 + 9 = 2^3 * 3^0 + 2^0 * 3^2`.
//
// Many integers have more than one valid partition, the first being `11` having the following two partitions.
// `11 = 2 + 9 = 2^1 * 3^0 + 2^0 * 3^2`
// `11 = 8 + 3 = 2^3 * 3^0 + 2^0 * 3^1`
//
// Let's define `P(n)` as the number of valid partitions of `n`. For example, `P(11) = 2`.
//
// Let's consider only the prime integers `q` which would have a single valid partition such as `P(17)`.
//
// The sum of the primes `q < 100` such that `P(q)=1` equals 233.
//
// Find the sum of the primes `q < 1000000` such that `P(q)=1`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// My ''bruteForce()'' attempt can verify `P(q < 100) = 1`: it creates a set of all powers `2^i 3^j < 100` and then analyzes each subset.
// Unfortuately there are 143 such powers for `q < 10^6` so that ''bruteForce()'' would have to check `2^143` subsets - that's impossible.
//
// So I "restarted" my thinking process: if I have a term `2^a 3^b` and then each other term `2^c 3^d` must be neither a multiple nor a divisor.
// That means if `a > c` then `b < d` because otherwise `2^a 3^b` would be a multiple of `2^c 3^d`.
// And if `a < c` then `b > d` because otherwise `2^a 3^b` would be a divisor of `2^c 3^d`.
//
// I am free to "sort" all elements of a partition and decided to apply the following rule:
// the first element has the smallest `2^x` and the largest `3^y` of all terms of the partition.
// That's exactly how the example is sorted (coincidence ?):
// `17 = 2^3 * 3^0 + 2^0 * 3^2`
//
// The first step is iterate over each possible `2^a * 3^b` so I can identify all numbers that can be reached by partitions consisting of only one term.
// Then I go through all numbers in ascending order:
// only if there was a way to reach a number with (at least) one term then I could add one more term - that must obey the rule if `a < c` then `b > d`
//
// My actual implementation may look a bit confusing because I tried hard to trim memory consumption:
// - a unique ID is assigned to each of 143 terms `2^a * 3^b < 10^6` (see container ''ids'')
// - create 143 bit vectors ''reachable'' with `10^6` elements: an element is true if there is at least  one partition and the "last" term carries that ID
// - create 143 bit vectors ''multiple''  with `10^6` elements: an element is true if there is more than one partition and the "last" term carries that ID
//
// The bit vectors ''reachable'' and ''multiple'' are filled step-by-step as I iterate over all `10^6` numbers.
// - ignore each ID of the current number ''i'' where ''reachable[id][i] == false''
// - else add one more term: iterate over each possible successor where `a < c` and `b > d`
// - set ''reachable[i + newTerm]'' accordingly, update ''multiple[i + newTerm]'' as well
// - if the current number ''i'' has at most one ''reachable[id][i]'' that is true (and even this must be ''multiple[id][i] == false'') then I found a unique partition
//
// # Note
// My final program was heavily affected by the desire to keep memory consumption low (it still needs about 38 MByte).
// The bit vectors ''reachable'' and ''multiple'' could be merged into a single vector of ''unsigned char'' that just counts all combinations.
// But this would require at least one byte per ID and per number, thus 143 MByte.
//
// The whole concept of using IDs could be replaced by temporarily allowing terms `2^a * 3^b > 10^6` and filtering them in the last step.
// However, this further increases memory consumption.
//
// # Alternative
// It seems you could write a Dynamic Programming algorithm - I tried but failed.

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

// ---------- standard prime sieve from my toolbox ----------

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
  const unsigned int half = (size >> 1) + 1;

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

// slow algorithm
unsigned int bruteForce(unsigned int limit)
{
  // create all terms 2^i * 3^j
  std::vector<unsigned int> terms;
  for (unsigned int two = 1; two <= limit; two *= 2)
    for (unsigned int three = 1; two * three <= limit; three *= 3)
      terms.push_back(two * three);

  // each term is a multiple of 1, so I can safely remove it
  terms.erase(terms.begin());
  // in ascending order
  std::sort(terms.begin(), terms.end());

  // mainly for debugging: keep track of all terms of all solutions
  typedef std::vector<unsigned int> Solution;
  std::vector<std::vector<Solution>> solutions(limit + 1);

  // analyze all subsets
  unsigned int maxMask = 1 << terms.size();
  for (unsigned int mask = 1; mask < maxMask; mask++)
  {
    unsigned int sum = 0;
    Solution currentTerms;
    // collect all terms
    for (unsigned int pos = 0; pos < terms.size(); pos++)
    {
      unsigned int posMask = 1 << pos;
      if ((posMask & mask) == 0)
        continue;

      // yes, use that term
      sum += terms[pos];
      currentTerms.push_back(terms[pos]);
    }

    // too large ?
    if (sum > limit)
      continue;

    // no term must a multiple of any other term
    bool isGood = true;
    for (unsigned int i = 0; i < currentTerms.size() && isGood; i++)
      for (unsigned int j = i + 1; j < currentTerms.size() && isGood; j++)
        isGood = (currentTerms[j] % currentTerms[i] > 0); // note: currentTerms was sorted ascendingly

                                                          // all terms are coprime
    if (isGood)
      solutions[sum].push_back(currentTerms);
  }

  // add all primes with exactly one solution
  unsigned int result = 0;
  for (unsigned int i = 0; i <= limit; i++)
    if (solutions[i].size() == 1 && isPrime(i))
    {
      // display terms
      auto current = solutions[i].front();
      std::cout << i << "=";
      for (auto x : current)
        std::cout << x << " ";
      std::cout << std::endl;

      result += i;
    }

  return result;
}

// return 2^two * 3^three
unsigned int power(unsigned int two, unsigned int three)
{
  // fast exponentiation for 3^three
  unsigned int result = 1;
  unsigned int base   = 3;
  while (three > 0)
  {
    if (three & 1)
      result *= base;
    base *= base;
    three >>= 1;
  }
  // multiplying by 2^two is a bitshift
  return result << two;
}

int main()
{
  unsigned int limit = 1000000;
  std::cin >> limit;

  // solve small inputs
  //std::cout << bruteForce(limit) << std::endl;

  // count how many powers of 2 and 3 are below the limit
  unsigned int numPowerOfTwo = 0;
  unsigned int numPowerOfThree = 0;
  while (power(numPowerOfTwo, 0)   <= limit)
    numPowerOfTwo++;
  while (power(0, numPowerOfThree) <= limit)
    numPowerOfThree++;

  // assign a unique ID to each 2^i * 3^j <= limit
  const unsigned char TooLarge = 0;
  unsigned char nextId = 1;
  unsigned char ids[30][20]; // more than big enough
  std::vector<std::pair<unsigned int, unsigned int>> reverseId(1); // 0 is dummy ID (TooLarge)
  for (unsigned int expTwo = 0; expTwo < numPowerOfTwo; expTwo++)
    for (unsigned int expThree = 0; expThree < numPowerOfThree; expThree++)
    {
      auto current = power(expTwo, expThree);
      if (current <= limit)
      {
        reverseId.push_back({ expTwo, expThree });
        ids[expTwo][expThree] = nextId++;
      }
      else
        ids[expTwo][expThree] = TooLarge;
    }
  // => only 143 out of 260 pairs

  // allocate memory, will store the number of ways to reach a value
  std::vector<std::vector<bool>> reachable(nextId);
  for (auto& x : reachable)
    x.resize(limit + 1, 0);
  std::vector<std::vector<bool>> multiple (nextId);
  for (auto& x : multiple)
    x.resize(limit + 1, 0);

  // seed values
  for (unsigned int id = 1; id < reverseId.size(); id++)
  {
    auto current = reverseId[id];
    auto value   = power(current.first, current.second);
    reachable[id][value] = true;
  }

  // add all primes with unique partitions
  fillSieve(limit);
  unsigned int sum = 0;
  for (unsigned int i = 1; i <= limit; i++)
  {
    // is there at least one combination to reach that number ?
    bool possible = false;
    // is there at most  one combination to reach that number ?
    bool unique   = true;
    // => look for numbers that are possible and unique

    for (unsigned int id = 1; id < reverseId.size(); id++)
    {
      auto expTwo   = reverseId[id].first;
      auto expThree = reverseId[id].second;

      // can I reach this number if one term has the current ID ?
      if (!reachable[id][i])
        continue;

      // store new sum
      if (possible || multiple[id][i]) // multiple ways to reach current position ?
        unique = false;
      possible = true;

      // try to add every possible coprime number with 2^c * 3^d (where c > a and b < d)
      for (unsigned int nextExpTwo = expTwo + 1; nextExpTwo < numPowerOfTwo; nextExpTwo++)
        for (unsigned int nextExpThree = 0; nextExpThree < expThree; nextExpThree++)
        {
          auto nextId = ids[nextExpTwo][nextExpThree];
          if (nextId == TooLarge)
            break;

          // add one term
          auto next = i + power(nextExpTwo, nextExpThree);
          if (next > limit)
            break;

          // if the next number was already reachable then there are now multiple ways
          if (reachable[nextId][next])
            multiple [nextId][next] = true;
          else
            reachable[nextId][next] = true;
        }
    }

    // unique sum of a prime ?
    if (possible && unique && isPrime(i))
      sum += i;
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
