// ////////////////////////////////////////////////////////
// # Title
// Robot Walks
//
// # URL
// https://projecteuler.net/problem=208
// http://euler.stephan-brumme.com/208/
//
// # Problem
// A robot moves in a series of one-fifth circular arcs (72&deg;), with a free choice of a clockwise or an anticlockwise arc for each step, but no turning on the spot.
//
// One of 70932 possible closed paths of 25 arcs starting northward is
//
// ![path](p208_robotwalk.gif)
//
// Given that the robot starts facing North, how many journeys of 70 arcs in length can it take that return it, after the final arc, to its starting position?
// (Any arc may be traversed multiple times.)
//
// # Solved by
// Stephan Brumme
// November 2017
//
// # Algorithm
// I played around with a 3rd party graphics library and tried to brute force the solution for 25 arcs:
// - I set up rotation matrices
// - need to adjust the allowed error to fix floating-point issues
// - and had tons of other problems
// Nevertheless, my program found those 70932 paths.
//
// More interesting is that I analyzed all paths with 5,6,7,...,25 arcs (obviously you need at least 5 arcs; I didn't analyze longer paths):
// - there are only solutions for 5,10,15,20 and 25 arcs, that means the path length must be a multiple of 5
// - the number of arcs "leaning left"  is always a multiple of 5
// - the number of arcs "leaning right" is always a multiple of 5
//
// Loosely speaking, each path consists only of full circles where the single parts are arbitrarily interleaved.
// For example: 25 arcs means I have 5 full circles, some clockwise and some anti-clockwise, and these parts of the circle are connected in a "random" way.
//
// The fascinating result of these observations: I don't need matrices at all - all I need is "counting" arcs !
// - the path always starts with arc 0
// - in each step I increment a counter in ''numVisited[currentArc]''
// - if the path turns left  then I continue with the next     arc (current arc plus  1 in modulo 5 ==> arc 1)
// - if the path turns right then I continue with the previous arc (current arc minus 1 in modulo 5 ==> arc -1 ==> arc 4)
//
// The same configurations of counters are repeatedly observed on different paths, so I can memoize them.
// The whole state consists of these five counters plus the current arc.
// I can merge these numbers into a single integer ''id'' and store it in a container ''cache''.
//
// Whenever I finish a path, all counters must have the same value.
// If the first four value are identical then the fifth must be, too.
// Instead of comparing the first four counters one-by-one I compare the first and second, the third and fourth and then the first and third.
// This way I save one comparison (it's somehow like binary search):
// "slow": ''numVisited[0] == numVisited[1] && numVisited[1] == numVisited[2] && numVisited[2] == numVisited[3] && numVisited[3] == numVisited[4]''
// "fast": ''numVisited[0] == numVisited[1] && numVisited[3] == numVisited[4] && numVisited[1] == numVisited[3]''
// ==> actually my code does the opposite thing because it want to detect those "failed" paths.
//
// # Note
// My initial version of the algorithm stored the memoized results in a hash-map.
// An ''std::vector'' is always faster at the cost of higher memory consumption, so I replaced it (see ''#define FAST'').
// I was surprised to see that the ''std::vector'' consumes actually 10% __less__ memory while being 6 times faster.

#include <iostream>
#include <vector>
#include <unordered_map>

unsigned char numVisited[5] = { 0,0,0,0,0 };
unsigned char maxPerArc = 0;

#define FAST
#ifdef  FAST
const auto Unknown = 0xFFFFFFFFFFFFFFFFull;
std::vector<unsigned long long> cache;
#else
std::unordered_map<unsigned int, unsigned long long> cache;
#endif

// count number of valid arcs
unsigned long long search(unsigned char arcsLeft = 70, unsigned char currentArc = 0)
{
  // all arcs were used
  if (arcsLeft == 0)
  {
    // did not return to initial arc ?
    if (currentArc != 0)
      return 0;

    // expect that all arcs were visited equally often
    if (numVisited[0] != numVisited[1] ||
        numVisited[2] != numVisited[3] ||
        numVisited[0] != numVisited[2]) // no need to check visited[4]
      return 0;

    return 1;
  }

  // create a unique key
  const auto IdFactor = maxPerArc + 1;
  unsigned int id = currentArc;
  id *= IdFactor;
  id += numVisited[0];
  id *= IdFactor;
  id += numVisited[1];
  id *= IdFactor;
  id += numVisited[2];
  id *= IdFactor;
  id += numVisited[3];
  id *= IdFactor;
  id += numVisited[4];
  // memoize
#ifdef FAST
  if (cache.empty())
    cache.resize(5 * IdFactor * IdFactor * IdFactor * IdFactor * IdFactor, Unknown);
  if (cache[id] != Unknown)
    return cache[id];
#else
  auto lookup = cache.find(id);
  if (lookup != cache.end())
    return lookup->second;
#endif

  unsigned long long result = 0;

  // turn left  if possible
  auto turnLeft = (currentArc + 1) % 5;
  if (numVisited[turnLeft]  < maxPerArc)
  {
    numVisited[turnLeft]++;
    result += search(arcsLeft - 1, turnLeft);
    numVisited[turnLeft]--;
  }

  // turn right if possible
  auto turnRight = (currentArc + 4) % 5; // -1 mod 5 => 5-1 mod 5
  if (numVisited[turnRight] < maxPerArc)
  {
    numVisited[turnRight]++;
    result += search(arcsLeft - 1, turnRight);
    numVisited[turnRight]--;
  }

  // save result
  cache[id] = result;
  return result;
}

int main()
{
  unsigned int numArcs = 70;
  std::cin >> numArcs;

  // let's go !
  maxPerArc = numArcs / 5;
  std::cout << search(numArcs) << std::endl;
  return 0;
}
