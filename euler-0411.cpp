// ////////////////////////////////////////////////////////
// # Title
// Uphill paths
//
// # URL
// https://projecteuler.net/problem=411
// http://euler.stephan-brumme.com/411/
//
// # Problem
// Let `n` be a positive integer. Suppose there are stations at the coordinates `(x, y) = (2^i mod n, 3^i mod n)` for `0 <= i <= 2n`.
// We will consider stations with the same coordinates as the same station.
//
// We wish to form a path from `(0, 0)` to `(n, n)` such that the `x` and `y` coordinates never decrease.
// Let `S(n)` be the maximum number of stations such a path can pass through.
//
// For example, if `n = 22`, there are 11 distinct stations, and a valid path can pass through at most 5 stations.
// Therefore, `S(22) = 5`. The case is illustrated below, with an example of an optimal path:
//
// ![path](p411_longpath.png)
//
// It can also be verified that `S(123) = 14` and `S(10000) = 48`.
//
// Find `sum{S(k^5)}` for `1 <= k <= 30`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// Generating the positions of all stations may look simple but there is a catch:
// a large number of stations share the same position and thus all of them have to be treated as a single station.
// STL's ''unique()'' function operates only on sorted data, therefore I need to call ''std::sort'' before it.
// Actually I remove duplicates every now and then to keep memory consumption low.
//
// When I looked at the number of stations before and after pruning duplicates I saw that these numbers stabilize after a while:
// there are cycles which will generate the same stations over and over again.
// My crude heuristic is to assume that if these numbers repeat then there is a cycle.
// You can probably detect these cycles easier/earlier in a more mathematical way.
//
// Then comes the fun part: finding the longest increasing subsequence (see https://en.wikipedia.org/wiki/Longest_increasing_subsequence )
// The Wikipedia page has a nice description of an efficient `O(n log n)` algorithm (unlike the weird posting http://www.geeksforgeeks.org/longest-monotonically-increasing-subsequence-size-n-log-n/
// and disturbingly ugly code at https://rosettacode.org/wiki/Longest_increasing_subsequence#C.2B.2B ).
// The core idea is simple:
// - create an array ''seqBack'' which will contain the last element (= ''back()'' of sequences)
// - those elements will be the smallest possible last elements among all sequences
// - the first station is a sequence, too (with length = 1) and obviously the smallest possible so far, store it at ''seqBack[0]''
// - for each station: find the first station that is bigger ("northern")
// - if there is no "bigger", then we just found a longer sequence, append it to ''seqBack''
// - replace the found station by the current station
// - the number of elements in ''seqBack'' is the longest subsequence
//
// Finding the shortest path now boils down to comparing only the y-component of the stations' locations.
// My first version of ''getLIS()'' called ''std::lower_bound()'' because that's the default behavior of the algorithm.
// However, there may be multiple stations with different x- but identical y-component and therefore I had to change it to ''std::upper_bound()''.
//
// # Note
// My solution doesn't finish within one minute. OpenMP is enable if you uncomment ''#define PARALLEL''.
// Depending on your number of CPU cores, the solution is found in less than 60 seconds.
//
// There are 19803868 unique stations for `n = 29^5`. I don't see how I can reduce the excessive memory consumption caused by that number.
// Only be replacing ''std::vector'' by ''std::deque'' and a clever copying strategy I managed to get barely under 256 MByte.
// And keep in mind that ''#define PARALLEL'' will further increase the memory usage.
//
// Right now my solution to this problem is ranked number one in my list of most the "expensive" algorithms (see my [performance](../performance/#slowandbig) ranking)
//
// And finally I decided to create a template version of my function ''getLIS()'' in case I need it for other projects or problems.

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

// ---------- powmod from my toolbox ----------

// return (base^exponent) % modulo for 32-bit values, no need for mulmod
unsigned int powmod(unsigned int base, unsigned int exponent, unsigned int modulo)
{
  unsigned int result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = (result * (unsigned long long)base) % modulo;

    // even exponent ? a^b = (a*a)^(b/2)
    base = (base * (unsigned long long)base) % modulo;
    exponent >>= 1;
  }
  return result;
}

// ---------- problem-specific code ----------

// a 2D point
struct Location
{
  // coordinates
  unsigned int x;
  unsigned int y;

  // create new point
  Location(unsigned int x_ = 0, unsigned y_ = 0)
  : x(x_), y(y_)
  {}

  // return point at (2^index % modulo, 3^index % modulo)
  static Location generate(unsigned int index, unsigned int modulo)
  {
    auto x = powmod(2, index, modulo);
    auto y = powmod(3, index, modulo);
    return Location(x, y);
  }

  // for sorting
  bool operator<(const Location& other) const
  {
    if (x != other.x)
      return x < other.x;
    else
      return y < other.y;
  }
  // for sorting
  bool operator==(const Location& other) const
  {
    return x == other.x && y == other.y;
  }
};

// return length of longest increasing subsequence
template <typename T>
unsigned int getLIS(T first, T last) // begin/end of a container
{
  // no data ?
  if (first == last)
    return 0;

  // seqBack[i] contains the smallest last element of a subsequence with length i + 1 (plus one because buffer is zero-indexed)
  std::vector<typename T::value_type> seqBack;
  seqBack.reserve(1 << 16);

  // initialize subsequence with length = 1 with the first element
  seqBack.push_back(*first++);
  while (first != last)
  {
    // locate shortest sequence where the last element >= current element
    auto current = *first++;
    auto pos = std::upper_bound(seqBack.begin(), seqBack.end(), current);
    // current element is the largest element observed so far ? => it extends the longest sequence
    if (pos == seqBack.end())
      seqBack.push_back(current);
    else // current element is smaller for that sequence length => "optimize"
      *pos = current; // could skip this step if *pos == current
  }

  return seqBack.size();
}

// compute S(n)
unsigned int minPathLength(unsigned int n)
{
  // generate all stations
  std::deque<Location> stations;
  //if (n > 10000000)
    //stations.reserve(n * 1.05);

  size_t lastUnsortedSize = 0;
  size_t lastSortedSize   = 0;

  for (unsigned int i = 0; i <= 2 * n; i++)
  {
    auto current = Location::generate(i, n);
    // the route always starts at (0,0) so discard any stations placed there
    if (current.x == 0 && current.y == 0)
      continue;

    stations.push_back(current);

    // 0x100000 is the sweet spot:
    // - if I remove duplicates more often, then I spent too much time sorting
    // - if I remove duplicates less often, then memory consumption grows (and sort/unique slow down as well)
    if (i % 0x100000 == 0 && i > 0)
    {
      // prune duplicates
      std::sort(stations.begin(), stations.end());
      auto garbage = std::unique(stations.begin(), stations.end());
      size_t unsortedSize = stations.size();
      size_t sortedSize   = std::distance(stations.begin(), garbage);
      stations.erase(garbage, stations.end());

      // same sizes as in last pass ? => assume that we entered a cycle/loop
      if (unsortedSize == lastUnsortedSize &&
          sortedSize   == lastSortedSize)
        break;

      lastUnsortedSize = unsortedSize;
      lastSortedSize   = sortedSize;
    }
  }

  // prune duplicates
  std::sort(stations.begin(), stations.end());
  auto garbage = std::unique(stations.begin(), stations.end());
  stations.erase(garbage, stations.end());

  // reduce locations to their y-components
  std::vector<unsigned int> onlyY;
  // simple but memory consuming
  //for (auto station : stations)
  //  onlyY.push_back(station.y);
  // a bit slower but keeps me below 256 MByte
  while (!stations.empty())
  {
    onlyY.push_back(stations.begin()->y);
    stations.erase(stations.begin());
  }

  // find shortest route
  return getLIS(onlyY.begin(), onlyY.end());
}

int main()
{
  unsigned int limit = 30;
  std::cin >> limit;

  unsigned int sum = 0;
//#define PARALLEL
#ifdef  PARALLEL
#pragma omp parallel for reduction(+:sum) schedule(dynamic)
#endif
  for (int k = limit; k >= 1; k--) // run loop in reverse to optimize load for OpenMP
    // evaluate k^5
    sum += minPathLength(k*k*k*k*k);

  std::cout << sum << std::endl;
  return 0;
}
