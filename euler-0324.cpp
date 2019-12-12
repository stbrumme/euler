// ////////////////////////////////////////////////////////
// # Title
// Building a tower
//
// # URL
// https://projecteuler.net/problem=324
// http://euler.stephan-brumme.com/324/
//
// # Problem
// Let `f(n)` represent the number of ways one can fill a `3 * 3 * n` tower with blocks of `2 * 1 * 1`.
// You're allowed to rotate the blocks in any way you like; however, rotations, reflections etc of the tower itself are counted as distinct.
//
// For example (with `q = 100000007`):
// `f(2) = 229`,
// `f(4) = 117805`,
// `f(10) mod q = 96149360`,
// `f(10^3) mod q = 24806056`,
// `f(10^6) mod q = 30808124`.
//
// Find `f(10^10000) mod 100000007`.
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// The tower consists of `n` levels, each consisting of 3x3 units (I call them ''layer'' throughout my code).
// Each layer is completely filled with blocks, each made of 2 units.
//
// When I look at a single layer in bird eye's view then these blocks can have four orientiations:
// - horizontal
// - vertical
// - up
// - down
//
// Both units of a horizontal or vertical block are located in the same layer:
// || 1 || 1 || 1 ||
// || H ++ H ++ . ||
// || . ++ . ++ V ||
// || . ++ . ++ V ||
//
// "Up"   blocks have one unit in the current layer and one unit in the layer above.
// "Down" blocks have one unit in the current layer and one unit in the layer below.
//
// Obviously, the bottom layer of the tower cannot contain any "down" blocks.
// And the top layer of the tower cannot contain any "up" blocks.
//
// My function ''createLayers'' recursively generates all possible layers.
// It fills are 9-character string with all combinations of ''-,|,U,D''.
// When inserting ''-'' (a horizontal block) or ''|'' (a vertical blocks) then it inserts two characters at once.
// The whole procedure is blazingly fast and the container ''layers'' stores all 3940 distinct layers.
//
// Not every layer can be placed on top of each other because "up" and "down" blocks must match.
// Therefore I added the concept of "borders":
// a "border" is a bitmask which is 1 for each unit which belongs to a block that is shared between two layers ("up"/"down" block).
// Each layer has two borders: its top and bottom borders. These borders have 9 bits because each layer has 3x3=9 units.
// Only if the n-th unit is "down" then the n-th bit of the bottom border is set.
// Only if the n-th unit is "up"   then the n-th bit of the top    border is set.
// That means that there are just `2^9 = 512` distinct borders.
// It's also easy to see that the bottom layer of the tower must be zero (no blocks "extending" into the ground).
// Moreover, the top layer of the tower is zero, too (flat roof).
//
// To verify my idea I wrote a simple ''bruteForce'' algorithm which is based on the concept of divide'n'conquer:
// - the top and bottom layer of the tower must be zero, so start with ''bruteForce(0,0,height)''
// - split the tower into an upper and a lower half (it doesn't have to be a 50:50 split !)
// - every possible border can be found between the upper and the lower half
// - iterate over all 512 borders and recursively call ''bruteForce'' twice: ''bruteForce(bottom,middle,height/2)'' and ''bruteForce(middle,top,height/2)''
// - every upper half is "compatible" to every lower half, so multiply their numbers
// - the number of layers for a tower with ''height = 1'' can be looked up in the ''borders'' container previously generated
//
// That algorithm verified the number of the test cases `f(2)`, `f(4)` and `f(10)`.
// But execution time was slow even with excessive use of memoization.
//
// A few days ago I solved problem 458 with a state machine / Markov chain and a matrix:
// - the number of transitions from border A to border B are already stored in the ''borders'' container
// - that's a 512x512 matrix
// - raise `M` to the `10^10000`-th power
// - since `10^10000` is too large to be done at once, I repeatedly exponentiate the matrix `10^19` times (that number barely fits in 64 bits) until arriving at `10^10000`
//
// There's a tiny problem: raising a 512x512 matrix to the `10^10000`-th power is extremely slow.
// Fast exponentiation helps, but those dimensions are still way too large.
// I ran the code before going to bed and the correct result popped up a few minutes after I woke up in the morning.
//
// I tried to reduce the number of borders (thus shrinking the matrix) but exploiting rotations but somehow messed up and couldn't reproduce the already found correct result.
// No idea where the bug was hidden ... but before I started fixing it I came up another idea:
// - many numbers in the 512x512 matrix are zero
// - after matrix exponentiation, the final result will be at (0,0) because the top and bottom borders must be zero
// - what if some parts of the matrix don't affect the value at (0,0) at all ?
//
// So I wrote the ''removeUnreachable'' function: starting at (0,0) it checks which cells of the matrix contribute to (0,0) directly and indirectly.
// It's a simple path-finding algorithm: is there a route between (0,0) and (x,y) where each intermediate step at (a,b) is not zero ?
// It turns out only 252 out 512 borders can be found in the tower at all, that's a reduction of about 50%.
// Since matrix multiplication is a `O(n^3)` algorithm, I cut down execution time by factor 8.
//
// My program still needed more than a minute so I improved the ''Matrix'' class and found a few speed-ups:
// - the matrix is symmetric, that means `M(x,y) = M(y,x)` (`M = M^T` for those who works more often with matrices)
// - therefore I only need to compute `M(x,y)` for `x <= y` and then just copy that value to `M(y,x)`
//
// ==> down to 6 minutes
//
// More or less by chance I printed the final matrix and still saw many numbers `M(x,y) = 0`.
// Fascinated by this observation, I ran the ''removeUnreachable'' function on the final matrix and it reduced the matrix to only 126x126.
// Then I aggressively tried to run ''removeUnreachable'' as often as possible and found:
// - after just one matrix exponentiation the matrix can be shrunk to 126x126, that means `M^2` can be reduced
// - no further reductions where found for `M^3`, `M^4` or other exponents
//
// Solving `f(10^10000)` - based on the reduced 126x126 matrix - finally takes less than a minute and is about as fast as the brute-code algorithm for `f(10)`.
//
// # Alternative
// The first values of the sequence `f` can be found in OEIS A028452 along with the generating function which is full of "magic constants".
// The generating function could probably be evaluated in milliseconds - in contrast to my solution which almost exceeds the time limit of 1 minute.
//
// The matrix can be further reduced to 23x23 if you write bug-free code properly handling rotations (as well as flipping).
// I don't quite understand the reasoning, but even a 19x19 matrix is discussed in the Project Euler forum.
//
// # Note
// Major parts of the ''Matrix'' class come from my solution of problem 458 (and were heavily optimized for the current problem).
// Aside from that this problem has [the largest chunk](/performance/#codemetrics) of code originally written for a specific problem.

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <tuple>

// everything modulo 10^9+7 (which is a prime)
const unsigned int Modulo = 100000007;

// different types of cells in a layer
const char Up         = 'U';
const char Down       = 'D';
const char Horizontal = '-';
const char Vertical   = '|';
const char Empty      = ' ';

// a single 3x3 level of the tower
typedef std::array<char, 9> Layer;
// store all different "designs" of completely filled layers, each is assigned a unique ID
std::set<Layer> layers;

// 2^9 different intersections between two layers
const unsigned int NumBorders = 1 << 9;
// remember how often each intersection can be observed
unsigned int borders[NumBorders][NumBorders] = { 0 };

// recursively create all different layers, add them to the "layers" container
void createLayers(Layer current)
{
  // 3x3 => 9 cells per layer
  // each cell is either 'U', 'D', 'H' or 'V'
  // cells which are not processed yet are ' '

  // find first empty cell
  bool full = true;
  unsigned int pos = 0;
  for (; pos < 9; pos++)
    if (current[pos] == Empty)
    {
      full = false;
      break;
    }

  // no empty cells ?
  if (full)
  {
    // std::set avoids duplicates
    layers.insert(current);
    return;
  }

  // attempt to insert a block crossing two layers
  current[pos] = Up;
  createLayers(current);
  //current[pos] = Empty;
  current[pos] = Down;
  createLayers(current);
  //current[pos] = Empty;

  // if the right neighbor is empty, too, then add a horizontal layer
  if (pos % 3 != 2 && current[pos+1] == Empty)
  {
    current[pos]   = Horizontal;
    current[pos+1] = Horizontal;
    createLayers(current);
    current[pos+1] = Empty;
    //current[pos] = Empty;
  }

  // and finally add a vertical layer
  if (pos < 6 && current[pos+3] == Empty)
  {
    current[pos]   = Vertical;
    current[pos+3] = Vertical;
    createLayers(current);
    //current[pos+3] = Empty;
    //current[pos] = Empty;
  }
}

// extract upper and lower border of a layer
void addBorders(Layer layer)
{
  // bitmasks for the top and bottom border of this layer, 1 means "crossing", 0 means "nope, not crossing"
  unsigned int top    = 0;
  unsigned int bottom = 0;
  for (unsigned int i = 0; i < 9; i++)
  {
    // extending into the layer above ?
    if (layer[i] == Up)
      top    |= 1 << i;
    // extending into the layer below ?
    if (layer[i] == Down)
      bottom |= 1 << i;
  }

  borders[bottom][top]++;
}

// count number of combinations of a (partial) tower where the bottom and top layer are known as well as the height
unsigned long long bruteForce(unsigned int maskBottom, unsigned int maskTop, unsigned int height)
{
  // degenerated case
  if (height == 0)
    return 0;
  // count how many layers have that special bottom and top mask
  if (height == 1)
    return borders[maskBottom][maskTop];

  // memoize
  typedef std::tuple<unsigned int, unsigned int, unsigned int> Id;
  static std::map<Id, unsigned long long> cache;
  Id id(maskBottom, maskTop, height);
  auto lookup = cache.find(id);
  if (lookup != cache.end())
    return lookup->second;

  // divide'n'conquer:
  // split the (partial) tower in half
  //auto half = height / 2;
  // it's slightly more efficient if it's not split 50:50 but one part is a power of two (access memoized data more frequently)
  unsigned int powerOfTwo = 1;
  while (powerOfTwo * 2 < height)
    powerOfTwo *= 2;

  auto heightTop    = powerOfTwo; // instead of "half"
  auto heightBottom = height - heightTop;

  unsigned long long result = 0;
  const unsigned int NumMasks = 1 << 9;
  for (unsigned int middle = 0; middle < NumMasks; middle++)
  {
    // each "half"-tower below the middle can be combined with each "half"-tower above the middle
    result += bruteForce(maskBottom, middle,  heightBottom) *
              bruteForce(middle,     maskTop, heightTop);
    result %= Modulo;
  }

  cache[id] = result;
  return result;
}

// quadratic 2D matrix with powmod, based on my solution of problem 458
template <typename Number>
class Matrix
{
  // store all elements as a flat 1D array
  std::vector<Number> data;
  // number of rows / columns
  unsigned int size_;

public:
  // set all elements to zero
  Matrix(unsigned int size__ = 1) // these underscores are sooooo ugly ...
  : data(size__ * size__, 0),
    size_(size__)
  { }

  // height / width of the quadratic matrix
  unsigned int size() const
  {
    return size_;
  }

  // access a field (read/write), indices are zero-based
  Number& operator()(unsigned int column, unsigned int row)
  {
    return data[column * size_ + row];
  }
  // access a field (read-only), indices are zero-based
  Number get(unsigned int column, unsigned int row) const
  {
    return data[column * size_ + row];
  }

  // multiply two matrices
  Matrix operator*(const Matrix& other) const
  {
    Matrix result(size_); // initially all fields are zero
    for (unsigned int i = 0; i < size_; i++)
      for (unsigned int j = 0; j < size_; j++)
      {
        if (other.get(i,j) == 0) // optional, just a performance tweak
          continue;

        for (unsigned int k = 0; k < size_; k++)
          result(i,k) += get(j,k) * other.get(i,j);
      }
    return result;
  }

  // multiply two symmetric matrices, with modulo
  Matrix multiplySymmetric(const Matrix& other, unsigned int modulo) const
  {
    Matrix result(size_); // initially all fields are zero

    // compute one half (without modulo)
    for (unsigned int i = 0; i < size_; i++)
      for (unsigned int j = 0; j < size_; j++)
        for (unsigned int k = i; k < size_; k++) // start at i instead of zero
          result(i,k) += get(j,k) * other.get(i,j);

    // copy to the other half of the matrix and compute modulo
    for (unsigned int i = 0; i < size_; i++)
    {
      // along the diagonal
      result(i,i) = result.get(i,i) % modulo;
      // and everything else
      for (unsigned int j = i + 1; j < size_; j++)
        result(j,i) = result(i,j) = result(i,j) % modulo;
    }
    return result;
  }

  // fast exponentiation with modulo
  Matrix powmod(unsigned long long exponent, unsigned int modulo) const
  {
    // more or less the same concept as powmod from my toolbox (which works on integers instead of matrices)

    // optional performance tweak
    if (exponent == 1)
      return *this;

    // start with the identity matrix
    Matrix result(size_);
    for (unsigned int i = 0; i < size_; i++)
      result(i,i) = 1;
    bool isIdentity = true;

    Matrix base = *this;

    while (exponent > 0)
    {
      // fast exponentation:
      // odd exponent ? a^b = a*a^(b-1)
      if (exponent & 1)
      {
        // optional optimization: avoid multiplying by the identity matrix
        if (isIdentity)
        {
          result = base;
          isIdentity = false;
        }
        else
        {
          //result = result * base
          result = result.multiplySymmetric(base, modulo);
        }
      }

      // even exponent ? a^b = (a*a)^(b/2)
      if (exponent > 1)
      {
        //base = base * base;
        base = base.multiplySymmetric(base, modulo);
      }

      exponent >>= 1;
    }
    return result;
  }
};

// find all states that can be reached from the initial state 0 and shrink matrix accordingly
Matrix<unsigned long long> removeUnreachable(const Matrix<unsigned long long>& matrix)
{
  // collect all reachable states
  std::set<unsigned int> reachable;
  std::set<unsigned int> todo = { 0 }; // 0 => initial state
  while (!todo.empty())
  {
    // pick next state
    auto current = *(todo.begin());
    todo.erase(current);
    reachable.insert(current);

    // put all new states on the todo list
    for (unsigned int i = 0; i < matrix.size(); i++)
      if (matrix.get(current,i) > 0 &&
          reachable.count(i) == 0) // avoid re-visited already processed states
        todo.insert(i);
  }

  // size of the new matrix
  auto matrixSize = reachable.size();
  if (matrixSize == matrix.size())
    return matrix; // no optimizations found ?

  // create a smaller matrix by skipping unreachable states
  Matrix<unsigned long long> smaller(matrixSize);

  // x and y will be consecutive while I skip a few state of i and j
  unsigned int x = 0;
  for (unsigned int i = 0; i < NumBorders; i++)
  {
    // ignore that column ?
    if (reachable.count(i) == 0)
      continue;

    unsigned int y = 0;
    for (unsigned int j = 0; j < NumBorders; j++)
    {
      // ignore that row ?
      if (reachable.count(j) == 0)
        continue;

      // copy cell
      smaller(x,y) = matrix.get(i,j);
      y++;
    }
    x++;
  }

  // done !
  return smaller;
}

int main()
{
  // this parameter has two different meanings:
  // - in brute-force mode it represents the height of the tower
  // - in "fast" mode the height of the tower is 10^limit
  unsigned int limit = 10000;
  std::cin >> limit;

  // start with an empty layer and search for all distinct ways to fill it
  Layer nothing;
  for (auto& x : nothing)
    x = Empty;
  createLayers(nothing);
  // => 3940 layers

  // extract the 512 distinct borders from these 3940 layers
  for (auto layer : layers)
    addBorders(layer);

  // top and bottom borders must be zero (no blocks "crossing" to the outside)
  const unsigned int InitialState = 0;
  const unsigned int FinalState   = 0;

//#define BRUTE_FORCE
#ifdef  BRUTE_FORCE
  // note: the height is just 10, not 10^10
  std::cout << bruteForce(InitialState, FinalState, limit) << std::endl;
  // f(6) % q = 64647289
  // f(8) % q = 69563725
  // f(10) takes about 40 seconds
  return 0;
#endif

  // copy to a matrix
  auto matrixSize = NumBorders;
  Matrix<unsigned long long> matrix(matrixSize);
  for (unsigned int i = 0; i < matrixSize; i++)
    for (unsigned int j = 0; j < matrixSize; j++)
      matrix(i,j) = borders[i][j];

  // remove unreachable "garbage" states, only 252 out of 512 states will be left
  matrix = removeUnreachable(matrix);

  // the first chunk (10^1) ...
  matrix = matrix.powmod(10, Modulo);
  // further reduction of states to 126
  matrix = removeUnreachable(matrix);

  // ... and the remaining 10^9999 chunks
  auto remaining = limit - 1;
  const auto AtOnce = 19; // process up to 10^18 per iteration
  while (remaining > 0)
  {
    // find largest number 10^x which still fits in a 64 bit integer
    unsigned long long power10 = 1;
    for (auto i = 1; i < AtOnce && remaining > 0; i++, remaining--)
      power10 *= 10;

    // thats where 99% of the total execution time is spent ...
    matrix = matrix.powmod(power10, Modulo);
  }

  // number of transitions from state 0 to state 0 (with 10^10000 layers inbetween)
  std::cout << matrix(InitialState, FinalState) << std::endl;
  return 0;
}
