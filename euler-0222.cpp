// ////////////////////////////////////////////////////////
// # Title
// Sphere Packing
//
// # URL
// https://projecteuler.net/problem=222
// http://euler.stephan-brumme.com/222/
//
// # Problem
// What is the length of the shortest pipe, of internal radius 50mm, that can fully contain 21 balls of radii 30mm, 31mm, ..., 50mm?
//
// Give your answer in micrometres (`10^{-6}` m) rounded to the nearest integer.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// This was a strange problem ... I came up with a solution that was slow and memory hungry.
// Then I saw an optimization and it became faster and even used less memory.
// But then I saw that I can basically solve it instantly - and wasn't willing the adapt the code accordingly.
//
// My solution isn't based on the fully optimized (I dare to say "trivial") algorithm.
// It basically tries to put all balls in various orders into the pipe and measures which combination is the shortest.
//
// The first observation is that the 3D objects (balls, pipe) can be reduced to their 2D equivalents (circles between two lines).
// If two balls `a` and `b` are stacked on top of each other, then their centres have a distance of `d = r_a + r_b`.
// I assumed that the pipe is vertical. Then a right triangle exists:
// the hypotenuse is `d = r_a + r_b`, one side is `x = (50 - r_a) + (50 - r_b) = 100 - (r_a + r_b)` and the other side `y` is:
// (1) `d^2 = x^2 + y^2`
// (2) `y^2 = d^2 - x^2`
// (3) `y = sqrt{d^2 - x^2}`
//
// Substituting `d` and `x`:
// (4) `y = sqrt{(r_a + r_b)^2 - (100 - (r_a + r_b))^2}`
//
// ==> This formula can be found in the function ''getDistanceY''.
//
// The function ''search'' enumerated all combinations and returns the correct result using Dynamic Programming.
// As mentioned before, my initial approach was working but was slow and memory hungry.
// Then it appeared to me that the two largest balls must be at the ends of the pipe (which was verified by ''search'').
// Fixing those two balls shrinks the size of the internal ''cache'' if ''size'' by factor 4 and speeds up the algorithm substantially.
//
// The code below does exactly that.
//
// # Alternative
// But then I thought: if I remove the two biggest balls - what happens to the rest ?
// Well, the same idea can be applied over and over again: pick the next two largest balls and put them next to the previously largest balls, and so on.
// In the end, the balls will be placed like this: 50mm, 48mm, 46mm, 44mm, ..., 43mm, 45mm, 47mm, 49mm.
// Generating that sequence can be done in a simple ''for''-loop and eliminates the need for ''search'' (and its large ''cache'').
// The expecting computation time will be close to zero.

#include <iostream>
#include <vector>
#include <cmath>

unsigned int minRadius = 30;
unsigned int maxRadius = 50;
unsigned int numBalls  = maxRadius - minRadius + 1;
unsigned int pipeRadius = 50;

// double has 64 bits => memoized cache would be twice as large
typedef float Number;

// an arbitrarily high number (more than sum of all radii)
const Number WorstCase = 999999999;

// get distance between centre of two balls along the y axis
Number getDistanceY(unsigned int radiusA, unsigned int radiusB)
{
  auto sum = radiusA + radiusB;
  return sqrt(sum*sum - (2*pipeRadius - sum)*(2*pipeRadius - sum));
}

// mask - bitmask of all available spheres
Number search(unsigned int mask, unsigned int lastRadius)
{
  // are all balls in the pipe (except for the largest) ?
  if (mask == 0)
    return getDistanceY(lastRadius, maxRadius) + maxRadius;

  // memoize
  const unsigned int Shift = numBalls - 2; // ignore the two largest balls
  const Number Invalid = -1;
  static std::vector<Number> cache((1 << Shift) * (maxRadius - minRadius + 1), Invalid);
  // already computed ?
  auto id = mask + ((lastRadius - minRadius) << Shift);
  if (cache[id] != Invalid)
    return cache[id];

  Number best = WorstCase;
  for (unsigned int radius = minRadius; radius <= maxRadius; radius++) // could stop at maxRadius - 2, too
  {
    // already used ?
    auto bit = 1 << (radius - minRadius);
    if ((mask & bit) == 0)
      continue;

    // process recursively
    auto newHeight = getDistanceY(radius, lastRadius) + search(mask & ~bit, radius);
    // improvement ?
    if (best > newHeight)
      best = newHeight;
  }

  cache[id] = best;
  return best;
}

int main()
{
  std::cin >> pipeRadius >> minRadius >> maxRadius;
  // avoid invalid input (for live test only)
  if (minRadius >= maxRadius)
    return 1;
  if (pipeRadius < maxRadius)
    return 2;

  numBalls  = maxRadius - minRadius + 1;

  // all bits set => all balls still available
  unsigned int mask = (1 << (maxRadius - minRadius + 1)) - 1;

  // the two largest balls must be at the ends of the pipe, remove those bits
  for (unsigned int radius = maxRadius - 1; radius <= maxRadius; radius++)
    mask &= ~(1 << (radius - minRadius));

  // start with the second largest ball
  auto first = maxRadius - 1;
  // half of the first ball (from its border to its centre) plus everything else
  auto best  = first + search(mask, first);

  // convert to micrometre and round to nearest integer
  std::cout << (unsigned int)round(1000 * best) << std::endl;
  return 0;
}
