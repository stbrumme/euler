// ////////////////////////////////////////////////////////
// # Title
// Counting Capacitor Circuits
//
// # URL
// https://projecteuler.net/problem=155
// http://euler.stephan-brumme.com/155/
//
// # Problem
// An electric circuit uses exclusively identical capacitors of the same value `C`.
// The capacitors can be connected in series or in parallel to form sub-units, which can then be connected in series or in parallel
// with other capacitors or other sub-units to form larger sub-units, and so on up to a final circuit.
//
// Using this simple procedure and up to `n` identical capacitors, we can make circuits having a range of different total capacitances.
// For example, using up to `n=3` capacitors of `60 \mu F` each, we can obtain the following 7 distinct total capacitance values:
//
// ![capacitors](p155_capacitors1.gif)
//
// If we denote by `D(n)` the number of distinct total capacitance values we can obtain when using up to `n` equal-valued capacitors
// and the simple procedure described above, we have: `D(1)=1`, `D(2)=3`, `D(3)=7` ...
//
// Find `D(18)`.
//
// Reminder : When connecting capacitors `C_1`, `C_2` etc in parallel, the total capacitance is `C_T = C_1 + C_2 + ...`,
// whereas when connecting them in series, the overall capacitance is given by: `dfrac{1}{C_T} = dfrac{1}{C_1} = dfrac{1}{C_2} + ...`
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// Each large circuit can be subdivided into two smaller circuits that are either connected in parallel or in series.
// There are sometimes multiple ways to subdivide a circuit - but all result in the same total capacitance.
// The top-most example can be split into 2 plus 1 or 1 plus 2 capacitors. When connected in parallel, I always get `120 + 60 = 60 + 120 = 180`.
// Of course that's a very basic example because both combinations are based on parallel connections.
//
// The idea is as follows:
// - there is a single capacitor for `D(1)`
// - to find all circuits for `D(2)` I have to combine all capacitors from `D(a)` and `D(b)` where `a+b=2` in parallel and in series
// - in general: to find all circuits for `D(c)` I have to combine all capacitors from `D(a)` and `D(b)` where `a+b=c` in parallel and in series
// - if different combinations give the same total capacitance then it only has to be counted once ("each capacitance must be unique")
//
// I was afraid of nasty rounding errors and wrote a class ''Fraction'' to be able to handle connections in series
// (because of the heavy use of divisions in `dfrac{1}{C_c} = dfrac{1}{C_a} = dfrac{1}{C_b}`).
//
// The numerator and/or denominator sometimes became pretty huge and after reading the problem statement multiple times
// I realized that the actual rating of a single capacitor doesn't matter:
// the example mentions `60 \mu F` but that Farad value is just arbitrary. The number of networks of capacitors with 1 F will be the same since
// I don't have to print the capacitances - only count how many different I can find.
// With `60 \mu F` I had tons of overflows, even with ''long long'', but `1 F` is perfectly fine with my ''Fraction'' class even when
// relying on the slightly faster ''unsigned short'' (==> 20% performance boost).
//
// # Note
// The class ''Fraction'' was added to my [toolbox](../toolbox/#fraction), too.
//
// A significant part of my code takes care that all solutions/capacitances are unique.
// The ''std::set'' container offers that "for free": yes, you don't need to write any code - unlike I did - but ''std::set'' is much slower, too.
// The problem is solved roughly five times faster when ''#define FAST'' is enabled (==> ''std::vector'' instead of ''std::set'').
// It's actually pretty nice how much code is identical for both containers: aside from ''push_back()'' vs. ''insert()'' there are no additional differences.
// There is also a significant memory overhead of ''set::set'': it consumes about 200% more memory even though it only stores 50% of the values.
//
// Even with all these optimizations: my solution is still in the top 10 of the "[most expensive solutions](../performance/#slowandbig)".
// And I have to admit: some parts of my code look ugly now - or better said: hardly comprehensible ...
//
// # Alternative
// The series can be found in https://oeis.org/A153588 .
//
// # Hackerrank
// My program is a little bit too slow for the last test case (probably misses the deadline just by a few milliseconds).
// It's strange that even the Hackerrank author suggests pre-computing the result to match the 2 seconds time limit.

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

// use std::vector instead of std::set
#define FAST

// ---------- code from my toolbox ----------

// simple class to representation a fraction
// note: no checks for a zero denominator
//       signs are not normalized
struct Fraction
{
  // change to long long if you need to support larger values
  typedef unsigned short T;

  // numerator
  T num;
  // denominator
  T den;

  Fraction(T numerator, T denominator = 1)
  : num(numerator), den(denominator)
  {}

  // add
  Fraction operator+(const Fraction& other) const
  {
    // n1/d1 + n2/d2 = (n1*d2 + n2*d1) / d1*d2
    return { T(num * other.den + other.num * den), T(den * other.den) };
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

  // return Fraction with swapped numerator and denominator
  Fraction inverse() const
  {
    return { den, num };
  }
};

// ---------- problem-specific code ----------

int main()
{
  unsigned int limit = 18;
  std::cin >> limit;

  // capacitance of a single element (1 F)
  Fraction Capacitance = 1;

  // store all circuits
#ifdef FAST
  std::vector<std::vector<Fraction>> circuits;
#else
  std::vector<std::set<Fraction>>    circuits;
#endif
  circuits.resize(limit + 1);

  // only one basic circuit at level 1 (I use 1F instead of 60uF but the exact value is irrelevant)
  circuits[1] = { Capacitance };

  // find all circuits consisting of 2,3,4, ... 18 elements
  for (unsigned int sizeC = 2; sizeC <= limit; sizeC++)
  {
    // split into "sub-circuits" A and B where size(A) <= size(B)
    for (unsigned int sizeA = 1; sizeA <= sizeC / 2; sizeA++)
    {
      // remaining circuits
      auto sizeB = sizeC - sizeA;

      // create all combinations
      for (auto circuitA : circuits[sizeA])
        for (auto circuitB : circuits[sizeB])
        {
          // serial connection
          // c = a + b
          auto serial = circuitA + circuitB;
#ifdef FAST
          circuits[sizeC].push_back(serial);
#else
          circuits[sizeC].insert(serial);
#endif

          // parallel connection
          // 1/c = 1/a + 1/b
          // invC = invA + invB where invC = 1/c and invA = 1/a and invB = 1/b
          auto parallel = (circuitA.inverse() + circuitB.inverse()).inverse();
#ifdef FAST
          circuits[sizeC].push_back(parallel);
#else
          circuits[sizeC].insert(parallel);
#endif
        }
    }

#ifdef FAST
    // remove duplicates
    std::sort(circuits[sizeC].begin(), circuits[sizeC].end());
    auto garbage = std::unique(circuits[sizeC].begin(), circuits[sizeC].end());
    circuits[sizeC].erase(garbage, circuits[sizeC].end());
#endif
  }

  // merge all circuits
  std::vector<Fraction> all;
#ifdef FAST
  // little trick to keep memory usage low: move the largest vector's content instead of copying
  all = std::move(circuits[limit]);
  limit--;
#endif
  // copy all capacitances
  for (unsigned int i = 1; i <= limit; i++)
    all.insert(all.end(), circuits[i].begin(), circuits[i].end());

  // and remove duplicates
  std::sort(all.begin(), all.end());
  auto garbage = std::unique(all.begin(), all.end());
  // count unique fractions
  auto numUnique = std::distance(all.begin(), garbage);
  std::cout << numUnique << std::endl;
  return 0;
}
