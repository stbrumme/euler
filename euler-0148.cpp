// ////////////////////////////////////////////////////////
// # Title
// Exploring Pascal's triangle
//
// # URL
// https://projecteuler.net/problem=148
// http://euler.stephan-brumme.com/148/
//
// # Problem
// We can easily verify that none of the entries in the first seven rows of Pascal's triangle are divisible by 7:
//
// ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||  3 ||
// ||    ||    ||    ||    ||    ||    ||  1 ||    ||    ||    ||    ||    ||    ||
// ||    ||    ||    ||    ||    ||  1 ||    ||  1 ||    ||    ||    ||    ||    ||
// ||    ||    ||    ||    ||  1 ||    ||  2 ||    ||  1 ||    ||    ||    ||    ||
// ||    ||    ||    ||  1 ||    ||  3 ||    ||  3 ||    ||  1 ||    ||    ||    ||
// ||    ||    ||  1 ||    ||  4 ||    ||  6 ||    ||  4 ||    ||  1 ||    ||    ||
// ||    ||  1 ||    ||  5 ||    || 10 ||    || 10 ||    ||  5 ||    ||  1 ||    ||
// ||  1 ||    ||  6 ||    || 15 ||    || 20 ||    || 15 ||    ||  6 ||    ||  1 ||
//
// However, if we check the first one hundred rows, we will find that only 2361 of the 5050 entries are not divisible by 7.
//
// Find the number of entries which are not divisible by 7 in the first one billion (109) rows of Pascal's triangle.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I needed a few attempts to finally solve this problem.
// My first idea was to iteratively create the triangle's rows, one at a time.
// To find the binomial coefficient C(n+1,k+1) you only need the previous row:
// `C(n+1,k+1) = C(n,k) + C(n,k+1)`
// To avoid overflows, each value should be the original value modulo 7.
// That works because `(a+b) mod 7 = ((a mod 7) + (b mod 7)) mod 7` (that's no special property of 7, you can pick any integer)
//
// Unfortunately I knew right from the start that this approach would be slow ... and memory-consuming (roughly 2 GByte).
// Nevertheless I used the code (see ''nextRow'') to show me the results for the first 1000 rows. And then a pattern appeared (my rows start at index 0):
// ||  4  ||   4   ||
// || row ++ found ||
// ||  0  ++   1   ||
// ||  1  ++   2   ||
// ||  2  ++   3   ||
// ||  3  ++   4   ||
// ||  4  ++   5   ||
// ||  5  ++   6   ||
// ||  6  ++   7   ||
// ||  7  ++   2   ||
// ||  8  ++   4   ||
// ||  9  ++   6   ||
// || 10  ++   8   ||
// || 13  ++  14   ||
// || 14  ++   3   ||
// || 15  ++   6   ||
//
// If I convert the row from decimal system to a number in base 7 then
// `found(row) = (digit_1(row) + 1) * (digit_2(row) + 1) * ... (digit_n(row) + 1)`
// For example: `found(15_10) = found(111_7) = (1+1) * (1+1) * (1+1) = 6`
//
// My function ''countNonDivisible'' does exactly that: it converts its parameter to base 7 and multiplies all digits plus one.
// It takes 34 seconds until the correct result is displayed on my computer.
//
// The inner-most loop of ''countNonDivisible'' consists of modulo and division operations - they are extremely slow in comparision to addition, subtractio, multiplication.
// And actually there is no need to perform these divisions:
// I process all numbers consecutively in ascending order.
//
// Therefore my final algorithm stores all digits of ''row'' in base 7. To speed up the program, not the true digits but the digits plus one are stored
// because when multiplying all digits I have to add one.
// That means that my array ''base7'' has up to 12 digits (`7^12 > 10^9`), each from ''1'' to ''7''.
// Incrementing by one can cause some digits to become ''7+1=8'' ==> reset them to 1 and carry over 1.
// That algorithm needs about 3.5 seconds.
//
// # Note
// A substantial reason for the performance gain of my third algorithm is that ''base7'' fits into the CPU cache and/or CPU registers (it's just 12 bytes).
// After changing the data type from ''std::vector<unsigned char>'' to ''std::vector<unsigned int>'' the execution time explodes from 3.5 to 28 seconds
// (and ''unsigned short'' ==> 3.9 seconds).
//
// # Alternative
// My final algorithm is still a brute-force algorithm. You can find a closed formula, too:
// The sum of the first `7^1 = 7` rows is `28`. The sum of the first `7^2 = 49` rows is `28^2 = 784` ... and so on.

#include <iostream>
#include <vector>

typedef std::vector<unsigned char> Row;
const unsigned int Modulo = 7;

// generate next row of Pascal's triangle modulo a number (> 1)
// return count of elements that are not a multiple of modulo (in C++ speak: x % modulo != 0)
unsigned long long nextRow(Row& row)
{
  // last value is always 1
  row.push_back(1);
  if (row.size() == 1)
    return 1;

  // first and last value are never a multiple of 7
  unsigned long long result = 2;

  // C(n+1,k+1) = C(n,k) + C(n,k+1)
  for (size_t k = row.size() - 2; k > 0; k--)
  {
    // note: I'm processing the row back-to-front
    // therefore minus 1 instead of plus 1
    unsigned char current = row[k] + row[k - 1];

    // subtraction is faster than modulo: current %= modulo
    // all values must be 0 ... 2*(modulo-1)
    if (current >= Modulo)
      current -= Modulo;

    // not divisible ?
    if (current != 0)
      result++;

    row[k] = current;
  }

  return result;
}

// convert to base 7 and multiply all digits plus 1
unsigned long long countNonDivisible(unsigned int row)
{
  unsigned long long result = 1;
  while (row > 0)
  {
    // one more digit ...
    result *= (row % Modulo) + 1;
    row    /= Modulo;
  }
  return result;
}

int main()
{
  unsigned int numRows = 1000000000;
  std::cin >> numRows;

  // for simple algorithm based on nextRow()
  Row current = { 1 };

  // for my fastest pseudo brute-force algorithm
  std::vector<unsigned char> base7(12, 1); // 7^12 > 10^9

  unsigned long long count = 1;
  for (unsigned int row = 1; row < numRows; row++)
  {
    // simple algorithm (basically takes forever and needs tons of memory)
    //auto found = nextRow(current);
    //std::cout << row << " " << found << std::endl;

    // slightly more advanced
    //auto found = countNonDivisible(row);

    // and my fastest (still pseudo brute-force) algorithm:
    // keep all digits of row in base 7 in an array base7 with a twist:
    // each digit is one higher than it should be
    // => because previously I had to add 1 before multiplying

    // next number
    base7[0]++;
    // carry over to next digits
    auto carryPos = 0;
    while (base7[carryPos] == Modulo + 1)
    {
      base7[carryPos] = 1; // remember: start at 1 instead of 0
      base7[carryPos + 1]++;
      carryPos++;
    }

    // multiply all digits
    unsigned long long found = 1;
    for (auto& x : base7)
      found *= x;

    // keep track of the sum of all rows
    count += found;
  }

  std::cout << count << std::endl;
  return 0;
}
