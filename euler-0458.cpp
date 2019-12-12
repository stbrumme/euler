// ////////////////////////////////////////////////////////
// # Title
// Permutations of Project
//
// # URL
// https://projecteuler.net/problem=458
// http://euler.stephan-brumme.com/458/
//
// # Problem
// Consider the alphabet `A` made out of the letters of the word "project": `A= \{c,e,j,o,p,r,t\}`.
// Let `T(n)` be the number of strings of length `n` consisting of letters from `A` that do not have a substring that is one of the 5040 permutations of "project".
// `T(7)=7^7 - 7! = 818503`.
//
// Find `T(10^12)`. Give the last 9 digits of your answer.
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// First let's state the most important property of the word "project": all its letters are distinct.
//
// I wrote three (!) algorithms but only the last actually solves `T(10^12)` in a reasonable amount of time (function ''fast''):
// ''bruteForce()'' was a quick hack to ensure I can solve `T(n)` for small parameters (`n < 10` is okay)
// It iterates over all possible `7^n` strings and checks every subset of 7 consecutive letters whether they can produce the word "project".
// I assign each letter P,R,O,J,E,C,T a number between 0 and 6 which in turn is used as the position in a bitmask.
// Only if seven consecutive letters are distinct then the ORed bitmask of those seven letters will be 1111111 in binary (`= 2^7 - 1 = 127` in decimal).
//
// The next algorithm's implementation is the function ''slow''. It works quite differently because it treats the whole thing as a state machine:
// - I count the number of strings __WITH__ the word "project"
// - ==> then the result is `7^n` minus that number of strings with the word "project"
// - in general: if the last `x` letters were distinct then the state machine is currently in state `x`
// - the state machine has 8 states (0 to 7) and starts in state 0
// - once state 7 is reached it stays there
//
// Initially, the variable ''state[8] = { 1,0,0,0,0,0,0,0 }'' because there is only one empty string and it's in state 0.
// No empty string can be in state 1,2,...,7.
//
// When the first  letter is processed then there are 7 different letters. Each causes a transition from state 0 to state 1.
// When the second letter is processed then there are 7 differenz letters as well but I have to be careful:
// if the second letter is equal to the first letter, then I remain in state 1 else I jump to state 2.
// Remember: the current state's ID indicates how many of the last letters were distinct.
//
// When the third letter is processed then there are 3 different state transitions:
// - if the new letter is equal to the first  letter, then the state machine stays in state 2
// - if the new letter is equal to the second letter, then the state machine must go back to state 1
// - if the new letter wasn't seen before then the state machine proceeds with state 3
// The same concept repeats with state 3,4,5,6.
//
// State 7 stands out because once I reach state 7 then I stay there, no matter what future letters will arrive.
// That's because I'm interesting in all strings containing the word "project" __at least once__ - and it doesn't matter if it appears 1x, 10x, 100x, ...
// I wrote a simple loop that repeatedly adds/multiplies each state by the number of different state transitions.
// That `O(n)` algorithm can solve `T(10^12)` in a few hours - still too slow !
//
// So I wrote the ''fast()'' function: the state transitions look a lot like a matrix - why not rewrite it as a ''Matrix'' ?
// The matrix's number at position x,y contains the number of transitions from state x to state y.
// For example, `M_{4,5} = ` ''M[4,5] = 3'' because 3 (out of 7) letters cause a transition from state 4 to 5.
// You'll find it in the fifth column and sixth line (the upper-right corner has index (0,0) because I used zero-based indices):
//
// `M = \begin{pmatrix} 0 & 0 & 0 & 0 & 0 & 0 & 0 & 0 \\`-
//                     `7 & 1 & 1 & 1 & 1 & 1 & 1 & 0 \\`-
//                     `0 & 6 & 1 & 1 & 1 & 1 & 1 & 0 \\`-
//                     `0 & 0 & 5 & 1 & 1 & 1 & 1 & 0 \\`-
//                     `0 & 0 & 0 & 4 & 1 & 1 & 1 & 0 \\`-
//                     `0 & 0 & 0 & 0 & 3 & 1 & 1 & 0 \\`-
//                     `0 & 0 & 0 & 0 & 0 & 2 & 1 & 0 \\`-
//                     `0 & 0 & 0 & 0 & 0 & 0 & 1 & 7 \end{pmatrix}`
//
// The ''state'' variable'' looks like a vector such that the computation per step is:
// (1) `state_{i+1} = M \bullet state_i`
//
// And the next step:
// (2) `state_{i+2} = M \bullet state_{i+1}`
// (3) `state_{i+2} = M \bullet (M \bullet state_i)`
// (4) `state_{i+2} = (M \bullet M) \bullet state_i`
// (5) `state_{i+2} = M^2 \bullet state_i`
//
// The last equation can be generalized to:
// (6) `state_{final} = M^n \bullet state_0`
//
// When solving problem 137 I wrote a fast exponentiation algorithm for a 2x2 matrix.
// The ''Matrix'' class written for the current problem generalizes that algorithm to larger quadratic matrices, see its ''powmod'' member function.
//
// I was too lazy to write a ''Vector'' class to represent the ''state'' variable - and I can get away with that lazy attitude ;-)
// The meaning of `M^n_{x,y}` was that the matrix `M` contains the total number of state transitions from state `x` to `y` after `n` steps.
// Hence `M^n_{0,7}` is the total number of strings __with__ the word "project" after those `n = 10^12` steps.
//
// Now I need `7^n mod 10^9` - and that number is part of the matrix, too !
// The number `M_{7,7}` is initially 7 and multiplied by itself `n` times, that's exactly what I need.
// However, I saw that `M^n_{7,7} = 1` after `n = 10^12` steps - which I found strange and I was sure I had a bug in my code.
// But Wolfram Alpha confirmed that it's correct: http://www.wolframalpha.com/input/?i=PowerMod%5B7,10%5E12,10%5E9%5D
// So the result is `M^n_{7,7} - M^n_{0,7}` (mod 1000000000).
//
// Fast exponentiation is truly fast and the correct result is displayed after a few milliseconds.
//
// # Alternative
// After submitting my result I looked at the official forum and noticed that pretty much everyone solved it the same way.
// But some postings differed completely: they found the [generating function](https://en.wikipedia.org/wiki/Generating_function) and solved it.

#include <iostream>
#include <array>

// "project" has 7 distinct letters
const auto WordSize = 7;
enum Letters { P = 0, R = 1, O = 2, J = 3, E = 4, C = 5, T = 6 };

// only the last nine digits
const unsigned int Modulo = 1000000000;

// ---------- first algorithm ----------

// super-slow brute-force algorithm, becomes unusable for numLetters > 10
unsigned long long bruteForce(unsigned int numLetters)
{
  // bruteForce( 7) =    818503
  // bruteForce( 8) =   5699280
  // bruteForce( 9) =  39688327
  // bruteForce(10) = 276386929
  unsigned long long result = 0;

  // 7^numLetters
  unsigned long long maxId = 1;
  for (unsigned long long exponent = 1; exponent <= numLetters; exponent++)
    maxId *= WordSize;

  // iterate over all 7^numLetters combinations
  for (unsigned long long i = 0; i < maxId; i++)
  {
    // convert the current number to letters (it's like converting from decimal to base 7)
    auto id = i;
    Letters letters[20];
    for (unsigned int letter = 0; letter < numLetters; letter++)
    {
      // I use the data type "Letters" to simplify debugging
      letters[letter] = (Letters)(id % WordSize);
      id /= WordSize;
    }

    // look at each group of 7 consecutive letters
    bool isProject = false;
    for (unsigned int from = 0; from + WordSize <= numLetters; from++)
    {
      // build a bitmask:
      unsigned int mask = 0;
      for (unsigned int current = 0; current < WordSize; current++)
        mask |= 1 << letters[from + current];

      // all seven letters used ?
      if (mask == 127) // 2^7 - 1 = 127
      {
        isProject = true;
        break;
      }
    }

    // no seven consecutive letters can be re-order to the word "PROJECT"
    if (!isProject)
      result++;
  }

  return result;
}

// ---------- second algorithm ----------

// O(n) algorithm, needs about 2 seconds for numLetters = 10^8
unsigned long long slow(unsigned long long numLetters)
{
  // state x means that currently x distinct letters were observed in the last x steps
  typedef std::array<unsigned long long, 7 + 1> State;
  // once we reach state 7 we have the word P,R,O,J,E,C,T (in any order) and remain in that state
  // no matter what follows
  State state = { 0 };
  state[0] = 1; // seed

  unsigned long long all = 1;
  for (unsigned long long i = 1; i <= numLetters; i++)
  {
    // total number of strings (regardless whether they contain PROJECT or not)
    all *= 7;
    all %= Modulo;

    State next = { 0 };

    // compute all state transitions
    next[1] = 7 * state[0] + 1 * state[1] + 1 * state[2] + 1 * state[3] + 1 * state[4] + 1 * state[5] + 1 * state[6];
    next[2] =                6 * state[1] + 1 * state[2] + 1 * state[3] + 1 * state[4] + 1 * state[5] + 1 * state[6];
    next[3] =                               5 * state[2] + 1 * state[3] + 1 * state[4] + 1 * state[5] + 1 * state[6];
    next[4] =                                              4 * state[3] + 1 * state[4] + 1 * state[5] + 1 * state[6];
    next[5] =                                                             3 * state[4] + 1 * state[5] + 1 * state[6];
    next[6] =                                                                            2 * state[5] + 1 * state[6];
    next[7] =                                                                                         + 1 * state[6];

    // once I'm in state 7 I stay there
    next[7] += 7 * state[7];

    // keep only the last 9 digits
    for (auto& x : next)
      x %= Modulo;

    state = std::move(next);
  }

  // state[7] represents the number of strings WITH the word "project"
  auto withProject = state[7];
  // without = all - with, but avoid negative results (due to modulo)
  if (all < withProject)
    all += Modulo;
  return all - withProject;
}

// ---------- third and final algorithm ----------

// quadratic 2D matrix
template <typename Number, unsigned int Size>
class Matrix
{
  // store all elements
  std::array<std::array<Number, Size>, Size> data; // same as Number data[Size][Size];

public:
  // set all elements to zero
  Matrix()
  : data()
  {
    for (unsigned int i = 0; i < Size; i++)
      data[i].fill(0);
  }

  // access a field (read/write), indices are zero-based
  Number& operator()(unsigned int column, unsigned int row)
  {
    return data[row][column];
  }
  // access a field (read-only), indices are zero-based
  Number get(unsigned int column, unsigned int row) const
  {
    return data[row][column];
  }

  // multiply two matrices
  Matrix operator*(const Matrix& other) const
  {
    Matrix result; // initially all fields are zero
    for (unsigned int i = 0; i < Size; i++)
      for (unsigned int j = 0; j < Size; j++)
        for (unsigned int k = 0; k < Size; k++)
          result(i,k) += get(j,k) * other.get(i,j);
    return result;
  }

  // fast exponentiation with modulo
  Matrix powmod(unsigned long long exponent, unsigned int modulo) const
  {
    // more or less the same concept as powmod from my toolbox (which works on integers instead of matrices)

    // start with identity matrix
    Matrix result;
    for (unsigned int i = 0; i < Size; i++)
      result(i,i) = 1;
    Matrix base = *this;

    while (exponent > 0)
    {
      // fast exponentation:
      // odd exponent ? a^b = a*a^(b-1)
      if (exponent & 1)
      {
        result = result * base;

        // modulo
        for (unsigned int i = 0; i < Size; i++)
          for (unsigned int k = 0; k < Size; k++)
            result(i,k) = result(i,k) % modulo;
      }

      // even exponent ? a^b = (a*a)^(b/2)
      base = base * base;

      // modulo
      for (unsigned int i = 0; i < Size; i++)
        for (unsigned int k = 0; k < Size; k++)
          base(i,k) = base(i,k) % modulo;

      exponent >>= 1;
    }
    return result;
  }
};

// solve "almost instantly", too fast to measure execution time ...
unsigned long long fast(unsigned long long numLetters)
{
  // same concept as slow() but rewritten with Matrix

  // state transitions
  Matrix<unsigned long long, 8> mat;
  mat(0,0) = 0; mat(1,0) = 0; mat(2,0) = 0; mat(3,0) = 0; mat(4,0) = 0; mat(5,0) = 0; mat(6,0) = 0; mat(7,0) = 0;
  mat(0,1) = 7; mat(1,1) = 1; mat(2,1) = 1; mat(3,1) = 1; mat(4,1) = 1; mat(5,1) = 1; mat(6,1) = 1; mat(7,1) = 0;
  mat(0,2) = 0; mat(1,2) = 6; mat(2,2) = 1; mat(3,2) = 1; mat(4,2) = 1; mat(5,2) = 1; mat(6,2) = 1; mat(7,2) = 0;
  mat(0,3) = 0; mat(1,3) = 0; mat(2,3) = 5; mat(3,3) = 1; mat(4,3) = 1; mat(5,3) = 1; mat(6,3) = 1; mat(7,3) = 0;
  mat(0,4) = 0; mat(1,4) = 0; mat(2,4) = 0; mat(3,4) = 4; mat(4,4) = 1; mat(5,4) = 1; mat(6,4) = 1; mat(7,4) = 0;
  mat(0,5) = 0; mat(1,5) = 0; mat(2,5) = 0; mat(3,5) = 0; mat(4,5) = 3; mat(5,5) = 1; mat(6,5) = 1; mat(7,5) = 0;
  mat(0,6) = 0; mat(1,6) = 0; mat(2,6) = 0; mat(3,6) = 0; mat(4,6) = 0; mat(5,6) = 2; mat(6,6) = 1; mat(7,6) = 0;
  mat(0,7) = 0; mat(1,7) = 0; mat(2,7) = 0; mat(3,7) = 0; mat(4,7) = 0; mat(5,7) = 0; mat(6,7) = 1; mat(7,7) = 7;
  // note: I could have skipped all those mat(x,y) = 0 because all cell are initialized with zero anyway

  // exponentiate matrix
  auto superMatrix = mat.powmod(numLetters, Modulo);

  // the number of strings WITH the word "project" (number of ways to transition from state 0 to 7)
  auto withProject = superMatrix.get(0,7);
  // total number of combinations is 7^numLetters
  // => I could use powmod(7, 10^12, Modulo) from my toolbox, but there's a 7 in the lower-right corner of the matrix at 7,7
  //    and since it's exponated as well, it becomes (7^numLetters) % Modulo and that's just what I need
  auto all         = superMatrix.get(7,7); // and surprisingly 7^(10^12) % 10^9 = 1 !

  // without = all - with, but avoid negative results (due to modulo)
  if (all < withProject)
    all += Modulo;
  return all - withProject;
}

int main()
{
  unsigned long long limit = 1000000000000ULL; // 10^12
  std::cin >> limit;
  //std::cout << bruteForce(limit) << std::endl;
  //std::cout << slow(limit) << std::endl;
  std::cout << fast(limit) << std::endl;
  return 0;
}
