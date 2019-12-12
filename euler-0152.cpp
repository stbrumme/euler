// ////////////////////////////////////////////////////////
// # Title
// Writing 1/2 as a sum of inverse squares
//
// # URL
// https://projecteuler.net/problem=152
// http://euler.stephan-brumme.com/152/
//
// # Problem
// There are several ways to write the number `1/2` as a sum of inverse squares using distinct integers.
//
// For instance, the numbers { 2,3,4,5,7,12,15,20,28,35 } can be used:
//
// `dfrac{1}{2} = dfrac{1}{2^2} + dfrac{1}{3^2} + dfrac{1}{4^2} + dfrac{1}{5^2} + dfrac{1}{7^2} + dfrac{1}{12^2} + dfrac{1}{15^2} + dfrac{1}{20^2} + dfrac{1}{28^2} + dfrac{1}{35^2}`
//
// In fact, only using integers between 2 and 45 inclusive, there are exactly three ways to do it, the remaining two being:
// { 2,3,4,6,7,9,10,20,28,35,36,45 } and { 2,3,4,6,7,9,12,15,28,30,35,36,45 }.
//
// How many ways are there to write the number `1/2` as a sum of inverse squares using distinct integers between 2 and 80 inclusive?
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// This problem would be extremely easy if we had less numbers. Nevertheless, the search works always the same:
// - try each combination of input values
// - use backtracking to add (and ignore) terms step-by-step:
// - if not all terms added yet but the sum already exceeds 1/2 then go back
// - if not all terms added yet but the sum is so small that it stays below 1/2 even if add all of the not-yet-analyzed values then go back
// ==> that's essentially how the function ''search()'' works
//
// Unfortunately this concept can solve the problem for `n = 45`. Therefore I have to find a clever way how I can skip some numbers.
// - the first observation is that the final denominator has to be 2 (and the numerator must be 1)
// - each denominator can be factored into prime numbers, they all "have to be gone", except for a single 2 of course
//
// If I have a (partial) sum `dfrac{a}{b}` then adding a single term `dfrac{1}{x^2}`:
// (1) `dfrac{a}{b} + dfrac{1}{x^2} = dfrac{a x^2 + b}{b x^2}`
//
// If `b` is coprime to `x^2` (which implies coprime to `x`) then I need to add at least one more term to get rid of `x^2`.
// That's a very helpful observation because adding `1 / 41^2` now is impossble - there is just no other term that also has prime factor 41.
// So the 41 would be stuck in the denominator - which is not allowed, there must be a single 2 at the end.
// The same can be said for all prime numbers `p > 80/2` which removes { 41,43,47,53,59,61,67,71,73,79 } from the list of candidates.
//
// Manual calculation showed that even 37 is "forbidden": the only remaining number with the same prime factor is 74.
// `1 / 37^2 + 1 / 74^2 = 5/5476` but `5476 = 2^2 * 37^2` (I asked Wolfram Alpha). So the 37 is still left in the denominator ...
// Then I wrote code to check for each prime number any combination of its multiples, e.g. for `p = 11` I checked all `2^7 = 128` combinations of { 11,22,33,44,55,66,77 }.
// and looked for a combination where the sum is not a multiple of the prime anymore ("sum mod prime != 0").
// This check is too slow for the primes 2 and 3 because there are `2^{floor{80/2}} = 2^40` or `2^{floor{80/3}} = 2^26` combinations.
// I just assumed that any number which has only prime factors 2 and/or 3 is "relevant".
//
// The output was astonishing: 11 and all prime numbers above 13 are "forbidden".
// The list of candidates can be reduced to all numbers whose prime factors are { 2,3,5,7,13 } and contains these 39 numbers:
// { 2,3,4,5,6, 7,8,9,10,12, 13,14,15,16,18, 20,21,24,27,28, 30,32,35,36,39, 40,42,45,48,52, 54,56,60,63,64, 65,70,72,80 }
//
// Looking at `2^39 approx 55 * 10^10` different combinations is a lot but manageable.
// Especially the optimizations mentioned at the beginning reduce this number considerably and I found the correct result in a bit more than 1 minute.
//
// Then I remembered the "trick" from problem 266 which I solved a few weeks ago:
// if pre-compute the sum of each subset of the highest numbers (''x > 40'' turns out to be a good choice) and store them in a container ''lastNumbers''
// then I can stop my search after reaching that threshold and lookup in ''lastNumbers'' how often the difference `dfrac{1}{2} - current` can be found.
// Now the program finishes after 0.3 seconds !
//
// # Note
// I kept track of the numbers actually used in all sums and it turns out 8 numbers were "useless": { 16,27,32,48,54,64,65,80 }
// With this "optimal" input set the program becomes twice as fast and finishes after 0.13 seconds.
//
// The "optimal" input set reveals an interesting pattern if I write down the prime factorization:
//{ `2^4`, `3^3`, `2^5`, `2^4 * 3`, `2 * 3^3`, `2^6`, `5 * 13`, `2^4 * 5` }
// I don't fully understand why all multiples of `2^4` and `3^3` are "useless" - but apparently there is a good reason.
//
// The ''Fraction'' class is part of my [toolbox](../toolbox/#fractions) and can add fractions, reduce them, etc.
// Some numbers becomes huge (before being reduced to a proper fraction) and I need GCC's 128 bit integer to find the correct result.
// That means my code doesn't compile with Visual C++.
//
// I could remove the ''members'' containers because it's only useful during debugging (to print exactly which solutions were found).
// But this problem was surprisingly hard and I just glad I solved it so I leave the code "as is". Well, I added a few comments afterwards.
// With over 300 lines it's one of my longest solutions, see [Code Metrics](../performance/#codemetrics).
//
// # Alternative
// If you multiply all fractions by `lcm(2,3,4,...,80)` then you don't need fractions at all.
//
// # Hackerrank
// The result doesn't need to be 1/2, it can be any number.
// And the number of terms can be changed which causes some problems because then even my 128 bit integers overflow.
// In the end about one third of all test cases are correct, one third times out and one third causes overflows.

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <cmath>

// uncomment to get more debugging output
//#define VERBOSE

// ---------- code copied from my toolbox ----------
// (I removed unused features to reduce source code size)

// simple class to representation a fraction
// note: no checks for a zero denominator
//       signs are not normalized
struct Fraction
{
  // change to long long if you need to support larger values
  typedef unsigned __int128 T;

  // numerator
  T num;
  // denominator
  T den;

  Fraction(T numerator = 0, T denominator = 1)
  : num(numerator), den(denominator)
  {}

  // add
  Fraction operator+(const Fraction& other) const
  {
    if (den == other.den)
      return Fraction(num + other.num, den);
    // n1/d1 + n2/d2 = (n1*d2 + n2*d1) / d1*d2
    return { num * other.den + other.num * den, den * other.den };
  }
  // subtract
  Fraction operator-(const Fraction& other) const
  {
    if (den == other.den)
      return Fraction(num - other.num, den);
    // n1/d1 - n2/d2 = (n1*d2 - n2*d1) / d1*d2
    return { num * other.den - other.num * den, den * other.den };
  }
  // sort
  bool operator< (const Fraction& other) const
  {
    // n1/d1 < n2/d2 => n1*d2 < n2*d2
    return num * other.den <  other.num * den;
  }
  // compare
  bool operator==(const Fraction& other) const
  {
    // n1/d1 < n2/d2 => n1*d2 < n2*d2
    return num * other.den == other.num * den;
  }

  // convert to a proper fraction
  Fraction reduced() const
  {
    // catch simple cases: 0/d and n/1
    if (num == 0 || den == 1)
      return { num, 1 };

    // divide numerator and denominator by their gcd()
    // code taken from my toolbox / gcd()
    auto a = num;
    auto b = den;
    while (a != 0)
    {
      auto c = a;
      a = b % a;
      b = c;
    }
    // now b contains the gcd()
    if (b == 1)
      return *this;
    return { num/b, den/b };
  }
};

// ---------- problem-specific code ----------

// sum of fractions should be exactly 1/2
Fraction sum(1, 2);

// all fractions that could be used
std::vector<unsigned int> candidates;
// store all combinations of all fractions beyond 1/threshold^2
unsigned int lastNumberThreshold = 40;
std::multiset<Fraction> lastNumbers; // a simple std::set would suffice for limit = 80 and lastNumberThreshold = 40
// sum of all fractions whose denominator exceeds a value (e.g. 40 => all fractions 1/40^2 + 1/42^2 + ... 1/80^2)
std::map<unsigned int, Fraction> remaining; // is equal to the largest key in lastNumbers

// track fractions of the current sum (debugging only)
std::vector<unsigned char> members;

// count how many combinations add up to 1/2
unsigned int search(Fraction current = Fraction(0, 1), size_t next = 0)
{
  // match !
  if (current == sum)
  {
#ifdef VERBOSE
    for (auto x : members)
      std::cout << int(x) << " + ";
    std::cout << std::endl;
#endif
    return 1;
  }

  // sum already too high ?
  if (sum < current)
    return 0;
  // no more terms ?
  if (next == candidates.size())
    return 0;

  // get next denominator (actually the whole fraction will be 1/number^2)
  auto number = candidates[next];

  // even if I choose all remaining number will it be enough to "reach" 1/2 ?
  auto maximum = current + remaining[number];
  if (maximum < sum)
    return 0;

  // lookup difference in all pre-computed sums of the last values
  if (number >= lastNumberThreshold)
  {
    // is there any sum matching the difference ?
    auto difference = sum - current;
    auto solutions  = lastNumbers.count(difference);

#ifdef VERBOSE
    if (solutions > 0)
    {
      for (auto x : members)
        std::cout << int(x) << " + ";
      std::cout << " (" << solutions << " solutions)" << std::endl;
    }
#endif

    return solutions;
  }

  // keep searching ...
  unsigned long long result = 0;
  // try to build the sum without number
  result += search(current, next + 1);

  // try to build the sum with    number
  Fraction add(1, number*number);
  members.push_back(number);
  current = current + add;
  current = current.reduced();
  result += search(current, next + 1);
  members.pop_back();

  return result;
}

int main()
{
  unsigned int denominator = 2;
  unsigned int limit = 80;
  std::cin >> denominator >> limit;
  sum.den = denominator;

  // find primes 2..80
  std::vector<unsigned int> primes;
  for (unsigned int i = 2; i <= limit; i++)
  {
    // basic trial division
    bool isPrime = true;
    for (unsigned int j = 2; j*j <= i; j++)
      if (i % j == 0)
      {
        isPrime = false;
        break;
      }
    if (isPrime)
      primes.push_back(i);
  }

  // determine relevant numbers
  std::set<unsigned int> relevantPrime;
  std::vector<bool> found(limit + 1, false);
  for (auto p : primes)
  {
    // enumarate 1/p^2, 1/(2p)^2, 1/(3p)^2, ...
    std::vector<Fraction> multiples;
    for (unsigned int m = p; m <= limit; m += p)
      multiples.push_back({ 1, m*m });

    // now check whether any combination is able to get rid of the prime in the denominator

    // go through all 2^n subsets
    auto combinations = 1 << multiples.size();
    for (auto mask = 1; mask < combinations; mask++)
    {
      // add all fractions
      Fraction current(0, 1);
      for (unsigned int pos = 0; pos < multiples.size(); pos++)
      {
        auto bit = 1 << pos;
        if ((bit & mask) == 0)
          continue;
        current = current + multiples[pos];
        current = current.reduced();
      }
      // and reduce them to a proper fraction
      current = current.reduced();

      // prime still there ? try another combination ...
      if (current.den % p == 0)
        continue;

      // same loop again, now mark all involved numbers as "found"
      for (unsigned int pos = 0; pos < multiples.size(); pos++)
      {
        auto bit = 1 << pos;
        if ((bit & mask) == 0)
          continue;

        // p is at position 0, 2p at 1, 3p at 2, ...
        auto isGood = (pos + 1) * p;
        found[isGood] = true;
      }

      // yes, it's a relevant prime
      relevantPrime.insert(p);
      found[p] = true;

      // analyze all combinations only for p >= 5 (special treatment for p=2 and p=3 follows below)
      if (p < 5)
        break;
    }
  }

  // assume that all numbers 2^n * 3^m need to be considered
  // (I only have to do this because the previous loop would be waaaay to slow for full evaluation of all cases)
  for (unsigned int two = 1; two <= limit; two *= 2)
    for (unsigned int three = 1; two*three <= limit; three *= 3)
      found[two * three] = true;

  // each suitable number between 2..80 must only consist of relevant primes
  for (unsigned int i = 2; i <= limit; i++)
  {
    // I don't need this number ?
    if (!found[i])
      continue;

    // factorize
    auto reduce = i;
    for (auto p : relevantPrime)
      while (reduce % p == 0)
        reduce /= p;

    if (reduce != 1)
    {
      found[i] = false;
      continue;
    }

    // yes, all prime factors found in relevantPrimes
    candidates.push_back(i);
  }

  // my code finds 2,3,4,5,6,7,8,9,10,12,13,14,15,16,18,20,21,24,27,28,30,32,35,36,39,40,42,45,48,52,54,56,60,63,64,65,70,72,80
  // but I analyzed all valid sums and found that only these numbers are actually needed:
  //candidates = { 2,3,4,5,6,7,8,9,10,12,13,14,15,   18,20,21,24,   28,30,   35,36,39,40,42,45,   52,   56,60,63,      70,72 };

#ifdef VERBOSE
  for (auto x : candidates)
    std::cout << x << ",";
  std::cout << std::endl;
#endif

  // for each number n compute the sum of 1/n^2 from n to 80
  // e.g. then I know for n = 9 whether I can reach 1/2 (or more) if I would use all remaining fractions
  // => it becomes obvious that I always need 2
  // => and I need 3 or 4
  // => helps me to skip analyzed many cases where the sum is just too small
  Fraction sum(0, 1);
  for (auto i = candidates.rbegin(); i != candidates.rend(); i++)
  {
    auto current = *i;
    sum = sum + Fraction(1, current * current);
    sum = sum.reduced();
    remaining[current] = sum;
  }

  // precompute sums of all combinations of the last numbers
  lastNumberThreshold = limit / 2; // arbitrary heuristic: all relevant numbers >= 40
                                   // it just turned out to be the fastest/best threshold ...
  // get range of the last numbers
  auto iteLastNumbers = candidates.begin();
  while (iteLastNumbers != candidates.end() && *iteLastNumbers < lastNumberThreshold)
    iteLastNumbers++;
  auto numLastNumbers = std::distance(iteLastNumbers, candidates.end());

  // go through all 2^numLastNumbers combinations
  unsigned int combinations = 1 << numLastNumbers;
  for (unsigned int i = 0; i < combinations; i++)
  {
    // add all fractions for each subset
    Fraction current(0, 1);
    for (unsigned int pos = 0; pos < numLastNumbers; pos++)
    {
      auto bit = 1 << pos;
      if ((i & bit) == 0)
        continue;

      auto add = *(iteLastNumbers + pos);
      current  = current + Fraction(1, add * add);
      current  = current.reduced();
    }
    // store for later use in search()
    lastNumbers.insert(current);
  }

  // let's go !
  std::cout << search() << std::endl;
  return 0;
}
