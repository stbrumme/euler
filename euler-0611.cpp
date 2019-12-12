// ////////////////////////////////////////////////////////
// # Title
// Hallway of square steps
//
// # URL
// https://projecteuler.net/problem=611
// http://euler.stephan-brumme.com/611/
//
// # Problem
// Peter moves in a hallway with `N+1` doors consecutively numbered from 0 through `N`.
// All doors are initially closed. Peter starts in front of door 0, and repeatedly performs the following steps:
//
// - First, he walks a positive square number of doors away from his position.
// - Then he walks another, larger square number of doors away from his new position.
// - He toggles the door he faces (opens it if closed, closes it if open).
// - And finally returns to door 0.
//
// We call an action any sequence of those steps. Peter never performs the exact same action twice,
// and makes sure to perform all possible actions that don't bring him past the last door.
//
// Let `F(N)` be the number of doors that are open after Peter has performed all possible actions.
// You are given that `F(5) = 1`, `F(100) = 27`, `F(1000) = 233` and `F(10^6) = 112168`.
//
// Find `F(10^12)`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// Yes, I got to admit: I just used brute force ... it solved the problem in under an hour and thus was my first time
// where I solved the most recent problem and became one of the 100 fastest solvers.
//
// Peter first walks to door `i^2`, takes a breath and then continues to door `i^2 + j^2` where `i > 0` and `j > i`.
// He toggles the door's state (closed ==> open, open ==> closed).
//
// All I need is to figure out a fast way to know how many doors have an odd number of ways to be represented as a sum of two squares.
// I read a few websites about "sum of two squares" (e.g. https://en.wikipedia.org/wiki/Fermat%27s_theorem_on_sums_of_two_squares ) but could deduce a simple counting formula.
//
// I process doors in chunks of `10^7` doors each, which requires 1.2 MByte per segment (''std::vector<bool>'').
// Then I iterate over all "relevant" pairs `i^2 + j^2` and negate the bit at that position.
// The term "relevant" means that `i^2 + j^2` has to be a value in the current segment: `from <= i^2 + j^2 < to`
// Even though I have to iterate over all `i`, I can restrict `j` to "match" the current segment:
// `j_{min} = \lceil sqrt{from - i^2} \rceil`
//
// # Note
// From the perspective of a software engineer (and that's what I do for a living) I did the right thing:
// instead of spending more than an hour on further reading, typing, debugging, etc., I kept the computer running for 49 minutes
// (with ''#define PARALLEL'' and ''numCores = 6'') and got the correct result.
// That's not the proper scientific way to solve a problem ... but a sound real-world approach !
//
// # Alternative
// As mentioned above, Fermat's theorem is the way to go. You need a prime-counting function for primes `4k+1`.
// I have prime-counting functions in my [toolbox](../toolbox/#primecounting) but they need to be modified to eliminate `4k+3` primes.

#include <iostream>
#include <vector>
#include <cmath>

// count open doors in a segment "from"-"to" (including "from", but excluding "to"
unsigned long long bruteForce(unsigned long long from, unsigned long long to)
{
  // allocate enough RAM
  auto size = to - from;
  std::vector<bool> doors(size, 0);
  // note: char instead of bool is about 15% faster but needs 700% more memory

  // look at all relevant squares i^2 + j^2
  for (unsigned long long i = 1; 2*i*i < to; i++)
  {
    // find smallest j such that i^2 + j^2 >= from
    auto minJ = i + 1;
    if (from > i*i + minJ*minJ)
      minJ = ceil(sqrt(from - i*i));

    // process all j until i^2 + j^2 >= to
    for (auto j = minJ; ; j++)
    {
      auto index = i*i + j*j;
      // range check
      if (index >= to)
        break;

      // adjust offset
      index -= from;
      // and "flip" that door
      doors[index] = !doors[index];
    }
  }

  // open doors => lowest bit is set
  unsigned long long result = 0;
  for (auto x : doors)
    result += x & 1; // x is odd => door is open

  return result;
}

int main()
{
  // 10^7 doors per segment
  const auto SliceSize =     100000000ULL;
  // 10^12 doors in total
  auto       limit     = 1000000000000ULL;
  std::cin >> limit;

  // total number of segments
  auto numSlices = limit / SliceSize; // => 10^12 / 10^7 = 10^5 by default
  if (numSlices == 0)
    numSlices++;

  // will contain the final result
  unsigned long long sum = 0;

  // 0 => all cores, 1 => one CPU, 2 => 2 CPUs, ... I used 6 on my 4+4-core system
#define PARALLEL
#ifdef  PARALLEL
  auto numCores = 6;
#pragma omp parallel for num_threads(numCores) reduction(+:sum) schedule(dynamic)
#endif
  for (unsigned int i = 0; i < numSlices; i++)
  {
    // compute lower/upper limit of current segment
    auto from = i * SliceSize;
    auto to   = from + SliceSize; // "to" is excluded
    if (to >= limit)
      to = limit + 1; // extend last segment to include "to" as well

    // process segment
    auto current = bruteForce(from, to);
    sum += current;

    // time echo "1000000000000" | ./611  <= numCores was set to 6
    // real    48m34.024s
    // user    290m21.924s
    // sys     0m49.327s

    // and for numCores = 1 (roughly half as fast)
    // real    94m51.945s
    // user    94m42.514s
    // sys     0m8.803s
  }

  std::cout << sum << std::endl;
  return 0;
}
