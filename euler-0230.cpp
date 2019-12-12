// ////////////////////////////////////////////////////////
// # Title
// Fibonacci Words
//
// # URL
// https://projecteuler.net/problem=230
// http://euler.stephan-brumme.com/230/
//
// # Problem
// For any two strings of digits, `A` and `B`, we define `F_{A,B}` to be the sequence `(A,B,AB,BAB,ABBAB,...)`
// in which each term is the concatenation of the previous two.
//
// Further, we define `D_{A,B}(n)` to be the `n`th digit in the first term of `F_{A,B}` that contains at least `n` digits.
//
// Example:
//
// Let `A=1415926535`, `B=8979323846`. We wish to find `D_{A,B}(35)`, say.
//
// The first few terms of `F_{A,B}` are:
// `1415926535`
// `8979323846`
// `14159265358979323846`
// `897932384614159265358979323846`
// `14159265358979323846897932384614159265358979323846`
//
// Then `D_{A,B}(35)` is the 35th digit in the fifth term, which is `9`.
//
// Now we use for `A` the first 100 digits of `pi` behind the decimal point:
//
// `14159265358979323846264338327950288419716939937510`
// `58209749445923078164062862089986280348253421170679`
//
// and for `B` the next hundred digits:
//
// `82148086513282306647093844609550582231725359408128`
// `48111745028410270193852110555964462294895493038196` .
//
// Find `sum_{n = 0,1,...,17} 10^n * D_{A,B}((127+19n) * 7n)` .
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I build a small lookup table of all Fibonacci numbers up to `2^64`.
//
// A simple loop from 17 to 0 processes one digit at a time.
// Note: I go from 17 to 0 instead of the more common 0 to 17 because then all digits are printed
// in the correct order and I can avoid multiplying them by `10^n`.
//
// The ''index'' formula needs to be adjusted by minus one because my indexing scheme starts at zero instead of one.
//
// Now comes the first step of the core algorithm:
// a simple loop looks for the first Fibonacci number that represents enough digits for ''index''.
// The value ''fibo'' will be incremented steadily, therefore I call it "step forward".
//
// The second step subdivides the current Fibonacci number in a left and right part in order to find out which
// building block (A or B) contains the desired digit.
// The value ''fibo'' will be decremented steadily, therefore I call it "step backwards".
//
// # Note
// I don't store the true Fibonacci numbers in ''fibonacci'' ==> I pre-multiply them by ''BlockSize''.
// It saves me a few multiplications when stepping forward and backwards and - more important to me - keeps the code cleaner.

#include <iostream>
#include <string>
#include <vector>

int main()
{
  // digits of pi
  std::string A = "1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";
  std::string B = "8214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196";

  std::cin >> A >> B;
  // live test only: detect invalid input
  if (A.size() != B.size() || A.empty())
    return 1;

  // 100 digits are one "block"
  const auto BlockSize = A.size(); // == B.size()

  // first two elements of the Fibonacci series (and dummy Fibonacci number 0 is 0)
  std::vector<unsigned long long> fibonacci = { 0, BlockSize };

  // find all Fibonacci numbers below 2^64
  while (fibonacci.back() < 1ULL << 63)
    fibonacci.push_back(fibonacci[fibonacci.size() - 2] + fibonacci[fibonacci.size() - 1]);

  // it's easier to reverse the loop and print all digits in the correct order ...
  for (auto n = 17; n >= 0; n--)
  {
    // compute index according to problem description
    unsigned long long index = 127 + 19*n;
    // times 7^n
    for (auto seven = 1; seven <= n; seven++)
      index *= 7;

    // my indexing scheme is zero-based but Project Euler's index starts at 1
    index--;

    // "step forward":
    // find the first Fibonacci number with enough digits
    unsigned int current = 1;
    while (index >= fibonacci[current])
      current++;

    // "step backwards":
    // deduce which Fibonacci number generated the part where the index is located
    while (true)
    {
      // reduced to a single block A or B ?
      if (current == 1)
      {
        std::cout << A[index];
        break;
      }
      if (current == 2)
      {
        std::cout << B[index];
        break;
      }

      // the mid-point divides left part (fibonacci minus 2) and right part (fibonacci minus 1)
      auto mid = fibonacci[current - 2];
      if (index < mid)
      {
        // left part
        current -= 2;
      }
      else
      {
        // right part
        index   -= mid;
        current -= 1;
      }
    }
  }

  std::cout << std::endl;
  return 0;
}
