// ////////////////////////////////////////////////////////
// # Title
// Scoring probabilities
//
// # URL
// https://projecteuler.net/problem=286
// http://euler.stephan-brumme.com/286/
//
// # Problem
// Barbara is a mathematician and a basketball player.
// She has found that the probability of scoring a point when shooting from a distance `x` is exactly `(1 - x/q)`,
// where `q` is a real constant greater than 50.
//
// During each practice run, she takes shots from distances `x = 1`, `x = 2`, ..., `x = 50` and, according to her records,
// she has precisely a 2% chance to score a total of exactly 20 points.
//
// Find `q` and give your answer rounded to 10 decimal places.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// My solution is based on Dynamic Programming and bisection search.
//
// The function ''probability'' returns the chance that exactly 20 are made from distances 1,2,...,50 given a certain `q`.
// And the ''main'' function iteratively narrows a range of potential values for `q` until the range is so small that I can be sure
// to be extremely close to the true value of `q` (error is less than `10^-10`).
//
// The bisection part was obvious to me right from the start.
// However, I needed a few minutes to figure out that I can use a Dynamic Programming approach (again :-) ) for ''probability''.
// I was worried that the cache size might grow too fast and tried to solve the problem analytically.
// But as it turns out the cache contains only about 33000 values at the end (even without clearing it when a new `q` is processed).
//
// # Alternative
// My simple recursion could be probably replaced by an iterative algorithm because of the low number of different states (''made, distance'').
//
// # Note
// I play basketball and in my opinion the problem is phrased incorrectly:
// according to the "Project Euler Basketball rules" Barbara gets only 1 point per shot she made.
// In real life, a shot is usually worth 2 points. You get 1 point for a free throw and 3 points for a "long-range" shot.
//
// The correct result is found after 39 iterations.
// Due to the nature of bisection, narrowing the initial interval doesn't really speed up the process.

#include <iostream>
#include <iomanip>
#include <map>

// Barbara hits exactly 20 times out of 50 attempted shots
      unsigned int threshold   = 20;
const unsigned int maxDistance = 50;
// Barbara has a 2% chance of making exactly 20 points (50 attempts)
const double chanceHitExactly  =  0.02;

// find probability of making 20 shots given a certain q
// note: second and third parameter are needed for internal recursion only
double probability(double q, unsigned int made = 0, unsigned int distance = 1)
{
  // made too many shots
  if (made > threshold)
    return 0;

  // finished ? => made exactly 20 shots ?
  if (distance > maxDistance)
    return made == threshold ? 1 : 0;

  // memoize
  static std::map<std::pair<unsigned int, double>, double> cache;
  auto id = std::make_pair(made * (maxDistance + 1) + distance, q);
  auto lookup = cache.find(id);
  if (lookup != cache.end())
    return lookup->second;

  // compute probabilities
  double chanceHit  = 1 - distance / q;
  double chanceMiss = 1 - chanceHit; // can be simplified to distance/q

  // go back one step
  distance++;
  // and attempt one more shot: could be a hit, could be a miss ...
  double result = chanceHit  * probability(q, made + 1, distance) +
                  chanceMiss * probability(q, made,     distance);

  cache[id] = result;
  return result;
}

int main()
{
  // user-defined number of made shots from distances 1..50
  std::cin >> threshold;

  // start with a large interval of potential values of q:
  // q >   50 because otherwise there would be a negative probability for the shot from distance 50
  double low  = maxDistance;
  // q <= 100 is a pretty random choice ... and it's much larger than the true value q
  double high = 100;

  // ten decimal places
  double accuracy = 0.0000000001;
  // while interval is still too wide ...
  while (high - low > accuracy)
  {
    // analyze its midpoint
    auto mid = (high + low) / 2;

    // below 2% ?
    if (probability(mid) < chanceHitExactly)
      high = mid; // mid is larger  than the true q
    else
      low  = mid; // mid is smaller than the true q
  }

  if (low > 50)
    // low == high (at least with respect to their first 10 decimal places)
    std::cout << std::fixed << std::setprecision(10) << low << std::endl;
  else
    std::cout << "impossible" << std::endl;

  return 0;
}
