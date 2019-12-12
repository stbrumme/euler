// ////////////////////////////////////////////////////////
// # Title
// Tours on a 4 x n playing board
//
// # URL
// https://projecteuler.net/problem=237
// http://euler.stephan-brumme.com/237/
//
// # Problem
// Let `T(n)` be the number of tours over a `4 * n` playing board such that:
// - The tour starts in the top left corner.
// - The tour consists of moves that are up, down, left, or right one square.
// - The tour visits each square exactly once.
// - The tour ends in the bottom left corner.
//
// The diagram shows one tour over a `4 * 10` board:
//
// ![tour](p237.gif)
//
// `T(10)` is 2329. What is `T(10^12) mod 10^8`?
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// A nice problem that you can easily understand within a few seconds. But it took a few days to come up with a solution ...
//
// Of course I immediately wrote a ''bruteForce'' algorithm and it solves the `T(10)` case. Anything beyond that is impossible.
//
// The main realization was to split the whole board in its columns.
// I identified 15 different columns (A - O) which can have a unique "flow" on their left and right border.
// The term "flow" means the chronological way how a piece moves across the board.
//
// The arrows symbolize the "flow" in and out of a column while hash signs stand for "no border crossing":
//
// ||    4    ||    4    ||    4    ||    4    ||    4    ||
// ||!   A    ++    B    ++    C    ++    D    ++    E    ||
// || ==> ==> ++ ==> ==> ++ ==> ==> ++ ==> ==> ++ ==> ##  ||
// || <== <== ++ <== ##  ++ ##  <== ++ <== <== ++ <== ##  ||
// || ==> ==> ++ ==> ##  ++ ##  ==> ++ ==> ##  ++ ==> ==> ||
// || <== <== ++ <== <== ++ <== <== ++ <== ##  ++ <== <== ||
//
// ||    4    ||    4    ||    4    ||    4    ||    4    ||
// ||!   F    ++    G    ++    H    ++    I    ++    J    ||
// || ==> ==> ++ ##  ==> ++ ==> ##  ++ ##  ==> ++ ==> ==> ||
// || <== <== ++ ##  <== ++ ##  ==> ++ ==> ##  ++ <== ##  ||
// || ##  ==> ++ ==> ==> ++ ##  <== ++ <== ##  ++ ##  ##  ||
// || ##  <== ++ <== <== ++ <== ##  ++ ##  <== ++ ##  <== ||
//
// ||    4    ||    4    ||    4    ||    4    ||    4    ||
// ||!   K    ++    L    ++    M    ++    N    ++    O    ||
// || ==> ##  ++ ==> ==> ++ ##  ==> ++ ==> ##  ++ ==> ##  ||
// || ##  ##  ++ ##  <== ++ ##  ##  ++ <== ##  ++ ##  ##  ||
// || ##  ==> ++ ##  ##  ++ ==> ##  ++ ==> ##  ++ ##  ##  ||
// || <== <== ++ <== ##  ++ <== <== ++ <== ##  ++ <== ##  ||
//
// Columns N and O can only be found at the right edge of the board.
//
// Unfortunately it's not sufficient to represent the flow by arrows because they are still ambigious:
// there are three different chronological orders how the "flow" can pass through column A.
//
// If I look at each column's left and right border then there are just 6 patterns for these borders.
// With a proper labelling of the flow's chronological order (indicated by 1,2,3,4 and a hash means "no crossing") I get 8 different borders:
//
// || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 ||
// || 1 ++ 1 ++ 1 ++ # ++ # ++ 1 ++ 3 ++ # ||
// || # ++ 2 ++ 2 ++ 1 ++ # ++ 4 ++ 2 ++ # ||
// || # ++ # ++ 3 ++ 2 ++ 1 ++ 3 ++ 1 ++ # ||
// || 2 ++ # ++ 4 ++ # ++ 2 ++ 2 ++ 4 ++ # ||
//
// The function ''fill()'' stores the borders of each column, e.g. column C is
// ''neighbors.insert( { "1##2", "1234" } );''
// Trust me, getting all this stuff right was a lot of work: I made tons of mistakes !
//
// I wrote two algorithms: a simple one that verifies `T(10)` and a much faster one to solve `T(10^12)`.
// ''slow()'' linearly goes through all borders that are allowed on the right side of the current border and stops if it reaches the right side of the board.
// Assuming that there are about 3 borders that are compatible in such a way, the routine analyzes `3^width` combinations.
// There's no way it can solve `T(10^12)` - but I really needed this algorithm to get my borders right.
// When the output finally matched the results of ''bruteForce'' I went on to write a faster (and more complex) algorithm.
//
// ''fast()'' is a divide-and-conquer approach:
// - I treat a group of columns as a blackbox where I only knows its left and right border
// - if I cut through this blackbox at an arbitrary point then any of the 8 borders could be found
// - well, that's not quite right, since the 8th border is reserved for the right-most border of the board ==> only 7 borders "inside" the blackbox
// - then the number of combinations of a blackbox is the product of its left and right half
// - if I keep doing this until the blackbox contains only a single column then I check whether this type of column is valid
//
// This isn't much faster than what ''slow()'' does ... but when the blackbox becomes smaller, I process the same kinds of blackboxes over and over again.
// Thus memoization drastically reduced the number of __different__ blackboxes. At the end, ''cache'' contains 3417 values.
//
// # Note
// Even though the result is found within about 0.02 seconds, I felt that dividing each blackbox in the middle isn't optimal:
// if I try to divide the blackbox in such a way that at least one half's size is a power of two (that means `2^i`) then ''cache'' contains only 2031 values.
// Moreover, the program runs about 50% faster.
//
// Replacing the ''std::string'' by plain integers would be still faster but I think it would be much harder to understand the code.
//
// # Alternative
// I was blown away by the simple solutions found by others: they discovered a relationship between `T(n)` and `T(n-1)`, ..., `T(n-4)`.
// Incredible stuff - or maybe just looked up in OEIS A181688.

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <tuple>

// assuming that the route could exceed the left border I get these states for the left-most and right-most borders:
typedef std::string Border;
const Border LeftBorder  = "1##2";
const Border RightBorder = "####";

// all possible borders that can be found inside the grid
std::set<Border> borders;
// define which borders can be next to each other
std::set<std::pair<Border, Border>> neighbors;

// set up the containers "borders" and "neighbors"
void fill()
{
  // the following lines are derived from my drawings above
  //                   left    right      column
  neighbors.insert( { "1234", "1234" } ); // A
  neighbors.insert( { "1432", "1432" } ); // A
  neighbors.insert( { "3214", "3214" } ); // A
  neighbors.insert( { "1432", "1##2" } ); // B
  neighbors.insert( { "3214", "1##2" } ); // B
  neighbors.insert( { "1##2", "1234" } ); // C
  neighbors.insert( { "1234", "12##" } ); // D
  neighbors.insert( { "1234", "##12" } ); // E
  neighbors.insert( { "12##", "1432" } ); // F
  neighbors.insert( { "##12", "3214" } ); // G
  neighbors.insert( { "1##2", "#12#" } ); // H
  neighbors.insert( { "#12#", "1##2" } ); // I
  neighbors.insert( { "12##", "1##2" } ); // J
  neighbors.insert( { "1##2", "##12" } ); // K
  neighbors.insert( { "1##2", "12##" } ); // L
  neighbors.insert( { "##12", "1##2" } ); // M
  neighbors.insert( { "1234", RightBorder } ); // N
  neighbors.insert( { "1##2", RightBorder } ); // O

  for (auto x : neighbors)
    borders.insert(x.first);
}

// fast search in O(log n)
unsigned long long search(const Border& left, const Border& right, unsigned long long length, unsigned int modulo)
{
  // reduced to a single column ?
  if (length == 1)
    // can these two borders be next to each other ?
    return neighbors.count(std::make_pair(left, right));

  // memoize
  auto id = std::make_tuple(left, right, length);
  // I don't add "modulo" to the key to keep it simple
  static std::map<std::tuple<Border, Border, unsigned long long>, unsigned long long> cache;
  auto lookup = cache.find(id);
  if (lookup != cache.end())
    return lookup->second;

  // split region into two parts: every possible border can be at the splitting point
  unsigned long long result = 0;
  for (const auto& next : borders)
  {
    // prefer a "power of two"-splitting, causes less states than splitting 50:50
    unsigned long long pow2 = 1;
    while (pow2 < length / 2)
      pow2 *= 2;
    //pow2 = length / 2; // alternatively: less efficient 50:50 method

    // process left  half
    auto leftHalf  = search(left, next,  pow2,          modulo);
    // process right half
    auto rightHalf = search(next, right, length - pow2, modulo);

    // each left half can be combined with each right half
    auto combined = (leftHalf * rightHalf) % modulo;
    result += combined;
  }

  result %= modulo;
  cache[id] = result;
  return result;
}

// slow search, no caching whatsoever
unsigned long long slow(const std::string& border, unsigned int length, unsigned int width, unsigned int modulo)
{
  // walked across the whole board ?
  if (length == width)
    return (border == RightBorder) ? 1 : 0;

  // proceed with each border that is compatible to the current one
  unsigned long long result = 0;
  for (auto x : neighbors)
    if (x.first == border)
      result += slow(x.second, length + 1, width, modulo);

  return result % modulo;
}

// backtracking of possible paths, doesn't need the information about borders etc.
typedef std::vector<std::vector<unsigned int>> Grid;
unsigned int bruteForce(Grid& grid, unsigned int x, unsigned int y, unsigned int step)
{
  // reached final position ?
  if (x == 0 && y == 3)
    return (step == grid.size() * grid[0].size()) ? 1 : 0;

  // take a step
  grid[x][y] = step;

  // try to search deeper in each direction
  unsigned int result = 0;
  if (x > 0 && grid[x - 1][y] == 0)
    result += bruteForce(grid, x - 1, y, step + 1);
  if (x + 1 < grid.size() && grid[x + 1][y] == 0)
    result += bruteForce(grid, x + 1, y, step + 1);
  if (y > 0 && grid[x][y - 1] == 0)
    result += bruteForce(grid, x, y - 1, step + 1);
  if (y < 3 && grid[x][y + 1] == 0)
    result += bruteForce(grid, x, y + 1, step + 1);

  // undo step
  grid[x][y] = 0;

  return result;
}

int main()
{
  // set up borders and their relationships
  fill();

  unsigned int       modulo = 100000000;
  unsigned long long limit  = 1000000000000;
  std::cin >> limit;

//#define BRUTEFORCE
#ifdef  BRUTEFORCE
  // allocate memory
  Grid grid(limit);
  for (auto& column : grid)
    column.resize(4, 0);
  // start in upper left corner (0,0), that's the first step
  std::cout << bruteForce(grid, 0, 0, 1) << std::endl;
#endif

//#define SLOW
#ifdef  SLOW
  std::cout << slow(LeftBorder, 0, limit, modulo) << std::endl;
#endif

#define FAST
#ifdef  FAST
  std::cout << search(LeftBorder, RightBorder, limit, modulo) << std::endl;
#endif

  return 0;
}
