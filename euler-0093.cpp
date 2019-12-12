// ////////////////////////////////////////////////////////
// # Title
// Arithmetic expressions
//
// # URL
// https://projecteuler.net/problem=93
// http://euler.stephan-brumme.com/93/
//
// # Problem
// By using each of the digits from the set, `{1, 2, 3, 4}`, exactly once, and making use of the four arithmetic operations (`+`, `-`, `*`, `/`) and brackets/parentheses,
// it is possible to form different positive integer targets.
//
// For example,
//
// `8 = (4 * (1 + 3)) / 2`
// `14 = 4 * (3 + 1 / 2)`
// `19 = 4 * (2 + 3) - 1`
// `36 = 3 * 4 * (2 + 1)`
//
// Note that concatenations of the digits, like `12 + 34`, are not allowed.
//
// Using the set, `{1, 2, 3, 4}`, it is possible to obtain thirty-one different target numbers of which 36 is the maximum,
// and each of the numbers 1 to 28 can be obtained before encountering the first non-expressible number.
//
// Find the set of four distinct digits, `a < b < c < d`, for which the longest set of consecutive positive integers, 1 to n,
// can be obtained, giving your answer as a string: `abcd`.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// No matter what brackets or operators are used, the basic pattern is always the same:
// 1. an operation on two out of the four numbers is executed
// 2. the result will become part of the equation which consists now of three numbers
// 3. an operation on two out of the three numbers is executed
// 4. the result will become part of the equation which consists now of two numbers
// 5. an operation on the final two numbers is executed
//
// I generate all sets of digits `{a, b, c, d}` where `a < b < c < d`. There are 126 such sets and four nested loops produce these sets in ''main''.
// My ''eval'' function accepts an arbitrary set of ''numbers'' and does the following:
// - pick any two numbers and execute all operations on these numbers, thus reducing ''numbers'' by one element
// - call itself recursively
// - when only one element is left in ''numbers'' then mark the result as ''used''
//
// There a a few pitfalls:
// - values may temporarily be non-integer, e.g. `(1/3)*6+7 = 9`, but may produce a valid integer result.
// - values may temporarily be negative, e.g. `(1-2)+3+4 = 6`, but may produce a valid positive integer result.
// - division by zero is not allowed, not even temporarily
// - any order of digits must be checked: `a op b` and `b op a`
//
// Especially the first pitfall (rational numbers) is tough to handle without floating-point numbers.
// However, ''double'' (or ''float'') may have some rounding issues, that's why I added an ''Epsilon'' to make sure that results like ''3.9999'' are treated as ''4''.
//
// ''getSequenceLength'' is the glue between ''main'' and ''eval'':
// It finds the first gap, that means the first number not represented by any combination of arithmetic operations.

#include <vector>
#include <iostream>
#include <cmath>

const double Epsilon = 0.00001;

// try all arithmetic operations of any two elements of "numbers", set their result in "used" to true
void eval(const std::vector<double>& numbers, std::vector<bool>& used)
{
  // 1. if array holds just one element, add it to the "used" list and we are done
  // 2. pick any two numbers
  // 3. loop through all operators
  // 4. add result to the array and call eval() recursively

  // step 1
  if (numbers.size() == 1)
  {
    auto result = numbers.front() + Epsilon;
    // reject non-integer result (caused by division)
    if (fmod(result, 1) > 10*Epsilon)
      return;

    int index = int(result + Epsilon);
    // reject negative and very large results
    if (index >= 0 && index < (int)used.size())
      used[index] = true;

    return;
  }

  // step 2
  auto next = numbers;
  for (size_t i = 0; i < numbers.size(); i++)
    for (size_t j = i + 1; j < numbers.size(); j++)
    {
      // fetch two numbers
      double a = numbers[i];
      double b = numbers[j];

      // prepare for next recursive step
      next = numbers;
      next.erase(next.begin() + j); // delete the higher index first
      next.erase(next.begin() + i);

      // steps 3 and 4 (unrolled)
      next.push_back(a + b); // add
      eval(next, used);
      next.back() = a - b;   // subtract (I)
      eval(next, used);
      next.back() = b - a;   // subtract (II)
      eval(next, used);
      next.back() = a * b;   // multiply
      eval(next, used);
      if (b != 0)
      {
        next.back() = a / b; // divide (I)
        eval(next, used);
      }
      if (a != 0)
      {
        next.back() = b / a; // divide (II)
        eval(next, used);
      }

      // note: I overwrite the last element because that's simpler than
      // pop_back(); push_back(...);
    }
}

// evaluate all expressions and count how many numbers from 1 to x can be expressed without any gaps
unsigned int getSequenceLength(const std::vector<double>& numbers)
{
  // find all results
  std::vector<bool> used(1000, false);
  eval(numbers, used);

  // longest sequence, beginning from 1
  unsigned int result = 0;
  while (used[result + 1])
    result++;
  return result;
}

int main()
{
#define ORIGINAL
#ifdef ORIGINAL
  unsigned int longestSequence = 0;
  unsigned int longestDigits   = 0;

  // four different digits
  for (unsigned int a =   1; a <= 6; a++)
    for (unsigned int b = a+1; b <= 7; b++)
      for (unsigned int c = b+1; c <= 8; c++)
        for (unsigned int d = c+1; d <= 9; d++)
        {
          // evaluate all combinations
          auto sequenceLength = getSequenceLength({ double(a), double(b), double(c), double(d) });

          // new record ?
          if (longestSequence < sequenceLength)
          {
            longestSequence = sequenceLength;
            // build one number out of the four digits
            longestDigits = a * 1000 + b * 100 + c * 10 + d;
          }
        }

  // print result
  std::cout << longestDigits << std::endl;

#else

  // 1..5 digits
  unsigned int numDigits;
  std::cin >> numDigits;
  // read those digits
  std::vector<double> numbers(numDigits);
  for (auto& x : numbers)
    std::cin >> x;

  // print length of longest sequence
  std::cout << getSequenceLength(numbers);

#endif

  return 0;
}
