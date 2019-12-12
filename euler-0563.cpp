// ////////////////////////////////////////////////////////
// # Title
// Robot Welders
//
// # URL
// https://projecteuler.net/problem=563
// http://euler.stephan-brumme.com/563/
//
// # Problem
// A company specialises in producing large rectangular metal sheets, starting from unit square metal plates.
// The welding is performed by a range of robots of increasing size. Unfortunately, the programming options of these robots are rather limited.
// Each one can only process up to 25 identical rectangles of metal, which they can weld along either edge to produce a larger rectangle.
// The only programmable variables are the number of rectangles to be processed (up to and including 25), and whether to weld the long or short edge.
//
// For example, the first robot could be programmed to weld together 11 raw unit square plates to make a 11x1 strip.
// The next could take 10 of these 11x1 strips, and weld them either to make a longer 110x1 strip, or a 11x10 rectangle.
// Many, but not all, possible dimensions of metal sheets can be constructed in this way.
//
// One regular customer has a particularly unusual order. He always demands that the finished product should have an exact area,
// and that the long side must not be more than 10% larger than the short side.
// If these requirements can be met in more than one way, in terms of the exact dimensions of the two sides, then he demands that all variants are produced.
// For example, if he were to ask for metal sheet of area 889200, then there are three final dimensions that can be produced: 900x988, 912x975 and 936x950.
// The target area of 889200 is the smallest area which can be manufactured in three different variants, within the limitations of the robot welders.
//
// Let `M(n)` be the minimal area that can be manufactured in exactly `n` variants with the longer edge not greater than 10% bigger than the shorter edge.
// Hence `M(3) = 889200`.
//
// Find `sum^100_{n=2}{M(n)}`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// The welding process start with a unit square. Using zero welding steps, these sizes are available:
// `S_0 = \{ 1 \}`
// The first welding machine can weld 2 to 25 of those unit square sheets:
// `S_1 = \{ 2,3,4,5,..,23,24,25 \}`
// The second welding machine can weld 2 to 25 sheets from the first step:
// `S_2 = \{ 2*2,2*3,2*4,...,2*24,2*25, \space 3*2,3*3,...,3*25, \space 4*2,...,4*25,...,25*2,25*3,...,25*25 \}`
//
// Some of those sheets have the same size, e.g. `2*3 = 3*2` or `2*4 = 8` (==> step 2 produces a sheet that has already been made in step 1 as well).
// After a few steps, these sizes are available:
// `S = \{ 2^a * 3^b * 4^c * 5^d * ... * 24^x * 25^y \}`
//
// Because of `24^x = 8^x * 3^x = 2^{3x} * 3^x` I can increase the number of steps (there's no limit of welding machines)
// and demand that all machines can only weld a prime numbers of sheets.
// We are left with 9 welding groups (the nine primes `<= 25`): 2, 3, 5, 7, 11, 13, 17, 19, 23
//
// That means that all areas `2^a 3^b 5^c 7^d 11^e 13^f 17^g 19^h 23^i` can be produced by the welding machines.
// When I generate them in ascending order I have to check their side lengths whether `a <= b` and `b / a <= 11 / 10 ==> 11a >= 10b` (`b` doesn't exceed `a` by more than 10%).
//
// I use a min-heap (implemented as a priority queue) to generate all such areas.
// Whenever the next area `A` is picked from the min-heap I find its highest prime factor `p_k` (it must be one of the nine mentioned above)
// and add `p_k * A`, `p_{k+1} * A`, ..., `23 * A` to the min-heap.
//
// Since each area `A` can be generated as Ax1 (a very long strip one unit wide and `A` units long), the potential side lengths are identical to the areas.
// Therefore I search for each area `A` all smaller areas `a` and `b`. The first edge case is `a = b` and the second is `b / a = 11 / 10` (which can be written as `11a = 10b`).
// The first edge case means `a = sqrt{A}`. I slowly decrement `a` and find all `b` such that `a * b = A` until `11a = 10b`.
//
// # Note
// I don't generate areas larger than 2300000000000000 because `M(100)` is smaller than that and I won't need those larger numbers at all.
//
// Looking at my log output I immediately saw that all `M()` are divisible by 10.
// And further analysis showed:
// - every `M(>= 2)` is divisible by 40
// - every `M(>= 9)` is divisible by 80
// - every `M(>= 53)` is divisible by 800 (which is in fact the 56th number found)
// I validated those rules for `M(2..100)` but they most likely apply for `M(>100)` as well.
// Running these three divisibility checks before computing the actual number of variants made my program about three times as fast (see ''#define FAST'').
//
// STL's ''priority_queue'' needs a container to store the data. The faster choice is ''std::vector'' (about 10% faster)
// but ''std::deque'' needs about 50% less memory because frequent reallocations are mostly avoided.

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <cmath>

int main()
{
  // highest number of combinations
  unsigned int maxCombinations = 100;
  std::cin >> maxCombinations;

  // problem asks for 2..100 => that means without M(1), just pretend it's already solved
  const auto NoSolution = 0ULL;
  unsigned int numSolutions = 1;
  std::vector<unsigned long long> solutions(maxCombinations + 1, NoSolution);

  // add all solutions
  unsigned long long result = 0;

  // create a min-heap
  typedef std::deque<unsigned long long> Container;
  //typedef std::vector<unsigned long long> Container; // a bit faster but needs almost 50% more memory
  std::priority_queue<unsigned long long, Container, std::greater<unsigned long long>> areas;

  // start with a single unit square
  areas.push(1);

  // each area will be equal to a side
  std::vector<unsigned long long> sides;

  // performance tweak: the correct result is below this value, don't generate larger numbers
  // => just to save memory and speed up the process
  const auto IgnoreAbove = 2300000000000000ULL;

  // until I found enough solutions
  while (numSolutions < maxCombinations)
  {
    // get next area
    auto current = areas.top();
    areas.pop();

    // will be the side length of a larger area, too
    if (current * current <= IgnoreAbove)
      sides.push_back(current);

    // insert all possible areas that can be produced by multiplying the current area with all primes between 2 and 25
    const unsigned char multiples[] = { 23,19,17,13,11,7,5,3,2 };
    for (auto multiple : multiples)
    {
      auto next = multiple * current;
      if (next <= IgnoreAbove)
        areas.push(next);

      // smaller primes were already taken care of (=> "next" already exists in "areas")
      if (current % multiple == 0)
        break;
    }

#define FAST
#ifdef  FAST
    // some heuristics based on the solutions I found
    if (numSolutions >= 56)
    {
      if (current % 800 != 0)
        continue;
    }
    else
    if (numSolutions >= 8)
    {
      if (current % 80 != 0)
        continue;
    }
    else
    if (current % 40 != 0)
      continue;
#endif

    // maximum length of the shorter side is sqrt(current area)
    auto iteLow = std::upper_bound(sides.begin(), sides.end(), (unsigned long long)sqrt(current));
    // upper_bound deliberately picks a value that is a tiny bit too large
    // one step back => longest possible length of the shorter side
    iteLow--;

    // look for all combinations
    unsigned int numFound = 0;
    //while (iteLow != areas.begin())
    while (iteLow != sides.begin())
    {
      // compute both sides
      auto shortSide = *iteLow--;
      auto longSide  = current / shortSide;

      // violating the 10% rule ?
      if (longSide * 10 > shortSide * 11)
        break;

      // is "current" indeed divisible by "shortSide" ? => then we have one more solution
      if (longSide * shortSide == current)
        numFound++;
    }

    // problem asks for 2..100 combinations only
    if (numFound < 2 || numFound > maxCombinations)
      continue;

    // new combination ?
    if (solutions[numFound] == NoSolution)
    {
      solutions[numFound] = current;
      result += current;
      numSolutions++;
      //std::cout << "found " << numFound << "/" << numSolutions << " @ " << current << " +" << result << std::endl;
    }
  }

  // show that large number !
  std::cout << result << std::endl;
  return 0;
}
