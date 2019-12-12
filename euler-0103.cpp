// ////////////////////////////////////////////////////////
// # Title
// Special subset sums: optimum
//
// # URL
// https://projecteuler.net/problem=103
// http://euler.stephan-brumme.com/103/
//
// # Problem
// Let `S(A)` represent the sum of elements in set `A` of size `n`. We shall call it a special sum set if for any two non-empty disjoint subsets,
// `B` and `C`, the following properties are true:
//
// i. `S(B) != S(C)`; that is, sums of subsets cannot be equal.
// ii. If `B` contains more elements than `C` then `S(B) > S(C)`.
//
// If `S(A)` is minimised for a given `n`, we shall call it an optimum special sum set.
// The first five optimum special sum sets are given below.
//
// `n = 1: { 1 }`
// `n = 2: { 1, 2 }`
// `n = 3: { 2, 3, 4 }`
// `n = 4: { 3, 5, 6, 7 }`
// `n = 5: { 6, 9, 11, 12, 13 }`
//
// It seems that for a given optimum set, `A = {a_1, a_2, ... , a_n}`, the next optimum set is of the form `B = {b, a_1+b, a_2+b, ... ,a_n+b}`, where `b` is the "middle" element on the previous row.
//
// By applying this "rule" we would expect the optimum set for `n = 6` to be `A = { 11, 17, 20, 22, 23, 24 }`, with `S(A) = 117`.
// However, this is not the optimum set, as we have merely applied an algorithm to provide a near optimum set.
// The optimum set for `n = 6` is `A = { 11, 18, 19, 20, 22, 25 }`, with `S(A) = 115` and corresponding set string: ''111819202225''.
//
// Given that A is an optimum special sum set for `n = 7`, find its set string.
//
// NOTE: This problem is related to Problem 105 and Problem 106.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// ''search'' recursively produces all ascending sequences where all elements are between ''minElement'' and ''maxElement''.
//
// ''check'' returns ''true'' if the set is "special" by analyzing all subsets:
// each number is either part or not part of a subset. That can be encoding by a single bit which is 0 (no) or 1 (yes).
// Then there are `2^size` combinations ==> I just run a counter named ''mask'' from ''0000000'' to ''1111111''
// (actually, ''0000000'' represents the empty set and can be skipped).
//
// For each sum I encounter along the way, I set a bit in ''sums''. It must never be set twice (first rule).
// Moreover, I track the lower and highest sum for each number of elements.
// If the highest sum of all n-element subsets is higher than then lowest sum of (n+1)-subsets then rule 2 was violated.
//
// All successfully verified sequences are stored in an ordered set ''solution'' and the first one is printed.
//
// # Hackerrank
// My simple brute-force approach produces only time-outs.
// There is a certain generating function I haven't fully figured out. All I see is that `x[2n+1] = x[2n]` and `x[2m] = x[2m-1] - ?`.

#define ORIGINAL
#ifdef  ORIGINAL

#include <iostream>
#include <vector>
#include <map>

typedef std::vector<unsigned int> Sequence;
std::map<unsigned int, Sequence> solutions;

// number of elements in full set
unsigned int finalSize;
// lower limit for the smallest element
unsigned int minElement;
// upper limit for the biggest element
unsigned int maxElement;

// return true if sequence is special
bool check(const Sequence& sequence)
{
  // sum of all elements
  unsigned int fullSum = 0;
  for (auto x : sequence)
    fullSum += x;

  // mark each generated sum as true, no collisions allowed
  std::vector<bool> sums(fullSum + 1, false);

  // track the lowest and highest sum for each subset size
  std::vector<unsigned int> maxSum(sequence.size() + 1, 0);
  std::vector<unsigned int> minSum(sequence.size() + 1, fullSum + 1);
  minSum[0] = maxSum[0] = 0; // empty set

  unsigned int fullMask = (1 << sequence.size()) - 1;

  // 2^elements iterations (actually, I ignore the empty set)
  for (unsigned int mask = 1; mask <= fullMask; mask++)
  {
    unsigned int sum  = 0;
    unsigned int size = 0;
    for (unsigned int element = 0; element < sequence.size(); element++)
    {
      // use that element ?
      unsigned int bit = 1 << element;
      if ((mask & bit) == 0)
        continue;

      sum += sequence[element];
      // count subset size
      size++;
    }

    // two subsets share the same sum ?
    if (sums[sum])
      return false;
    sums[sum] = true;

    // adjust lowest and highest sum of current subset
    if (minSum[size] > sum)
      minSum[size] = sum;
    if (maxSum[size] < sum)
      maxSum[size] = sum;
  }

  // make sure that no set will fewer elements has a higher sum
  for (size_t i = 1; i < sequence.size(); i++)
    if (maxSum[i] > minSum[i + 1])
      return false;

  // yes, have another solution
  solutions[fullSum] = sequence;
  return true;
}

// enumerate all sequences where each number is between minElement and maxElement and a_i > a_j if i > j
void search(Sequence& sequence) // manipulate in-place
{
  // enough elements ? check whether it is "special"
  if (sequence.size() == finalSize)
  {
    check(sequence);
    return;
  }

  // all numbers which are bigger than the predecessor
  auto last = sequence.empty() ? minElement - 1 : sequence.back(); // minus 1 because last always adds 1
  for (unsigned int i = last + 1; i <= maxElement; i++)
  {
    sequence.push_back(i);

    // go deeper ...
    search(sequence);

    sequence.pop_back();
  }
}

int main()
{
  // read set size
  std::cin >> finalSize;

  // crude heuristics for the lower and upper limits
  minElement = 1;
  maxElement = 10;
  if (finalSize >= 5)
  {
    maxElement = finalSize * finalSize;
    minElement = maxElement / 4;
  }

  Sequence empty;
  search(empty);

  // `n = 1: {  1 }`
  // `n = 2: {  1,  2 }`
  // `n = 3: {  2,  3,  4 }`
  // `n = 4: {  3,  5,  6,  7 }`
  // `n = 5: {  6,  9, 11, 12, 13 }`
  // `n = 6: { 11, 18, 19, 20, 22, 25 }`
  for (auto s : solutions)
  {
    for (auto x : s.second)
      std::cout << x;
    break;
  }

  return 0;
}

#else

// heavily modified Hackerrank problem

#include <iostream>
#include <vector>

typedef std::vector<unsigned int> Sequence;

// apply near-optimal algorithm to increase sequence by one element
// my algorithm builds the sequence in reverse
void next(Sequence& sequence) // modify in-place
{
  auto middle = sequence[(sequence.size() - 1) / 2];
  for (auto& x : sequence)
    x = (x + middle) % 1000000007;
  sequence.push_back(middle);
}

int main()
{
  unsigned int length;
  std::cin >> length;

  Sequence sequence = { 1 };
  sequence.reserve(length);
  for (unsigned int i = 2; i <= length; i++)
    next(sequence);

  for (auto i = sequence.rbegin(); i != sequence.rend(); i++)
    std::cout << *i << " ";
  std::cout << std::endl;
  return 0;
}

#endif
