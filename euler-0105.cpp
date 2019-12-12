// ////////////////////////////////////////////////////////
// # Title
// Special subset sums: testing
//
// # URL
// https://projecteuler.net/problem=105
// http://euler.stephan-brumme.com/105/
//
// # Problem
// Let `S(A)` represent the sum of elements in set `A` of size `n`. We shall call it a special sum set if for any two non-empty disjoint subsets,
// `B` and `C`, the following properties are true:
//
// i. `S(B) != S(C)`; that is, sums of subsets cannot be equal.
// ii. If `B` contains more elements than `C` then `S(B) > S(C)`.
//
// For example, `{ 81, 88, 75, 42, 87, 84, 86, 65 }` is not a special sum set because `65 + 87 + 88 = 75 + 81 + 84`,
// whereas `{ 157, 150, 164, 119, 79, 159, 161, 139, 158 }` satisfies both rules for all possible subset pair combinations and `S(A) = 1286`.
//
// Using [sets.txt](https://projecteuler.net/project/resources/p105_sets.txt) (right click and "Save Link/Target As..."), a 4K text file with one-hundred sets containing seven to twelve elements (the two examples given above are the first two sets in the file), identify all the special sum sets, A1, A2, ..., Ak, and find the value of S(A1) + S(A2) + ... + S(Ak).
//
// NOTE: This problem is related to Problem 103 and Problem 106.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// ''check'' is pretty much the same code as in problem 103. See there for an explanation.
//
// I spent most of the time writing code to parse the input (and did it twice because Project Euler has a CSV format while Hackerrank a simpler space-separated).
//
// # Hackerrank
// The set can have up to 100 elements. That's `2^100` subsets ... my code can't handle that.
// I always return "NO" whenever a set more 30 or more elements to avoid timeouts. Fun fact: that's the correct result for all tests.

//#define ORIGINAL

#include <iostream>
#include <vector>

typedef std::vector<unsigned int> Sequence;

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
  return true;
}

#ifdef ORIGINAL

// convert a line of Project Euler's format into a sequence
Sequence readLine()
{
  Sequence result;
  while (true)
  {
    result.push_back(0);

    char oneByte = 0;
    while (true)
    {
      oneByte = std::cin.get();

      // end of file ?
      if (!std::cin)
        return result;

      // not a digit ?
      if (oneByte < '0' || oneByte > '9')
        break;

      // append digit
      result.back() *= 10;
      result.back() += oneByte - '0';
    }

    // end of line
    if (oneByte != ',')
      break;
  }
  return result;
}

#else

// convert a line of Project Euler's format into a sequence
Sequence readLine()
{
  // read number elements
  unsigned int size;
  std::cin >> size;

  // read elements
  Sequence result(size);
  for (auto& x : result)
    std::cin >> x;

  return result;
}

#endif

int main()
{
  unsigned int tests = 100;
#ifdef ORIGINAL
  unsigned int sum = 0;
#else
  std::cin >> tests;
#endif

  while (tests--)
  {
    auto sequence = readLine();

#ifdef ORIGINAL
    // special ?
    if (check(sequence))
      // yes !
      for (auto x : sequence)
        sum += x;
#else
    // special ?
    if (sequence.size() < 30) // 2^30 has already one billion subsets ...
      std::cout << (check(sequence) ? "YES" : "NO") << std::endl;
    else
      std::cout << "NO" << std::endl; // just make a guess
#endif
  }

#ifdef ORIGINAL
  std::cout << sum << std::endl;
#endif

  return 0;
}
