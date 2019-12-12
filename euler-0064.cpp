// ////////////////////////////////////////////////////////
// # Title
// Odd period square roots
//
// # URL
// https://projecteuler.net/problem=64
// http://euler.stephan-brumme.com/64/
//
// # Problem
// All square roots are periodic when written as continued fractions and can be written in the form:
// `sqrt{N} = a_0 + dfrac{1}{a_1 + frac{1}{a_2 + frac{1}{a_3 + ...}}}`
//
// For example, let us consider `sqrt{23}`:
// `sqrt{23} = 4 + sqrt{23} - 4 = 4 + dfrac{1}{frac{1}{sqrt{23}-4}} = 4 + dfrac{1}{1 + frac{sqrt{23}-3}{7}}`
//
// If we continue we would get the following expansion:
// `sqrt{23} = 4 + dfrac{1}{1 + dfrac{1}{3 + dfrac{1}{1 + frac{1}{8 + ...}}}}`
//
// The process can be summarised as follows:
// `a_0 = 4, dfrac{1}{sqrt{23}-4} = dfrac{sqrt{23}+4}{7} = 1 + dfrac{sqrt{23}-3}{7}`
//
// `a_1 = 1, dfrac{7}{sqrt{23}-3} = dfrac{7(sqrt{23}+3)}{14} = 3 + dfrac{sqrt{23}-3}{2}`
//
// `a_2 = 3, dfrac{2}{sqrt{23}-3} = dfrac{2(sqrt{23}+3)}{14} = 1 + dfrac{sqrt{23}-4}{7}`
//
// `a_3 = 1, dfrac{7}{sqrt{23}-4} = dfrac{7(sqrt{23}+4)}{7} = 8 + dfrac{sqrt{23}-4}{1}`
//
// `a_4 = 8, dfrac{1}{sqrt{23}-4} = dfrac{sqrt{23}+4}{7} = 1 + dfrac{sqrt{23}-3}{7}`
//
// `a_5 = 1, dfrac{7}{sqrt{23}-3} = dfrac{7(sqrt{23}+3)}{14} = 3 + dfrac{sqrt{23}-3}{2}`
//
// `a_6 = 3, dfrac{2}{sqrt{23}-3} = dfrac{2(sqrt{23}+3)}{14} = 1 + dfrac{sqrt{23}-4}{7}`
//
// `a_7 = 1, dfrac{7}{sqrt{23}-4} = dfrac{7(sqrt{23}+4)}{7} = 8 + dfrac{sqrt{23}-4}{1}`
//
// It can be seen that the sequence is repeating. For conciseness, we use the notation `sqrt{23} = [4;(1,3,1,8)]`, to indicate that the block `(1,3,1,8)` repeats indefinitely.
//
// The first ten continued fraction representations of (irrational) square roots are:
//
// `sqrt{2} = [1;(2)]`, period=1
// `sqrt{3} = [1;(1,2)]`, period=2
// `sqrt{5} = [2;(4)]`, period=1
// `sqrt{6} = [2;(2,4)]`, period=2
// `sqrt{7} = [2;(1,1,1,4)]`, period=4
// `sqrt{8} = [2;(1,4)]`, period=2
// `sqrt{10} = [3;(6)]`, period=1
// `sqrt{11} = [3;(3,6)]`, period=2
// `sqrt{12} = [3;(2,6)]`, period=2
// `sqrt{13} = [3;(1,1,1,1,6)]`, period=4
//
// Exactly four continued fractions, for `N <= 13`, have an odd period.
// How many continued fractions for `N <= 10000` have an odd period?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// I didn't know anything about continued fractions before I saw this problem. The Wikipedia article is pretty long: https://en.wikipedia.org/wiki/Continued_fraction
// Even more important, there is another Wikipedia article about the relationship of square roots and continued fractions: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Continued_fraction_expansion
//
// The whole algorithm can be found in my function ''getPeriodLength''.
// My first step is to find the integer part of the square root. That's pretty easy:
// ''root = sqrt(x)'' ==> if ''root*root == x'' then ''x'' is a perfect square and we can abort.
//
// In the example above, `x=4` and `root=4`. Even though `a_0` was the first step, there is a step before it (kind of `a_{-1}`):
// `sqrt{23} = 0 + dfrac{sqrt{23} - 0}{1}`
// The variable ''numerator = 0'' refers to the zero which is subtracted in the numerator.
// The variable ''denominator = 1'' refers to the 1 in the denominator.
// The variable ''a = root'' will be `a_0`.
//
// I do the following to compute `a_1`:
// `numerator = denominator * a - numerator` ==> `numerator_1 = 1 * 4 - 0 = 4`
//
// `denominator = \lfloor dfrac{x - numerator^2}{denominator} \rfloor` ==> `denominator_1 = \lfloor dfrac{23 - 4^2}{1} \rfloor = 7`
//
// `a = \lfloor dfrac{root + numerator}{denominator} \rfloor` ==> `a_1 = \lfloor dfrac{4 + 4}{7} \rfloor = 1`
//
// As you can see in the problem statement, the first line contained the fraction `dfrac{sqrt{23}+4}{7}`
//
// And for `a_2`:
// `numerator_2 = 7 * 1 - 4 = 3`
//
// `denominator_2 = \lfloor dfrac{23 - 3^2}{7} \rfloor = 2`
//
// `a_2 = \lfloor dfrac{4 + 3}{2} \rfloor = 3`
//
// The mathematical reasoning is based on the general concept that `(a - b) * (a + b) = a^2 - b^2`.
// If `a = sqrt{x}` and `b = y` then `(sqrt{x} - y) * (sqrt{x} + y) = x - y^2`
//
// For `x = 23` and `y = \lfloor sqrt{x} \rfloor = \lfloor sqrt{23} \rfloor = 4`:
// `dfrac{1}{(sqrt{23} - 4)}`
//
// `= dfrac{sqrt{23} + 4}{(sqrt{23} - 4) * (sqrt{23} + 4)}`
//
// `= dfrac{sqrt{23} + 4}{23 - 4^2}`
//
// `= dfrac{sqrt{23} + 4}{7}`
//
// Then the largest possible integer such that 0 < numerator < denominator:
// `= dfrac{sqrt{23} + 4 - 7 + 7}{7}`
//
// `= 1 + dfrac{sqrt{23} - 3}{7}`
//
// A loop can be identified by keeping track of the tupel ''(a, numerator, denominator)''. If it appears a second time, we have entered a loop.
// We could create a data structure - something like ''std::set<std::tupel>'' - but Wikipedia mentions a neat trick that I don't understand:
// The equation ''a == 2 * root'' becomes true as soon as we enter a loop.

#include <cmath>
#include <iostream>

// return length of period or 0 for perfect squares
unsigned int getPeriodLength(unsigned int x)
{
  // without any fractional part yet ...
  unsigned int root = sqrt(x);

  // exclude perfect squares (no period)
  if (root * root == x)
    return 0;

  // the integer part of sqrt(x)
  unsigned int a = root;
  // let's use a variable numerator to store what we subtract
  unsigned int numerator   = 0; // initially zero, e.g. 4 will appear in second iteration of sqrt(23)
  unsigned int denominator = 1; // initially one,  e.g. 7 will appear in second iteration of sqrt(23)

  // count how long it takes until the next period starts
  unsigned int period = 0;

  // terminate when we see the same triplet (a, numerator, denominator) a second time
  // to me it wasn't obvious that this happens exactly when a == 2 * root
  // but thanks to Wikipedia for that trick ...
  while (a != 2 * root)
  {
    numerator   = denominator * a - numerator;
    // must be integer divisions !
    denominator = (x - numerator * numerator) / denominator;
    a           = (root + numerator) / denominator;

    period++;
  }

  return period;
}

int main()
{
  unsigned int last;
  std::cin >> last;

  // count all odd periods
  unsigned int numOdd = 0;
  for (unsigned int i = 2; i <= last; i++) // 0 and 1 are perfect squares
  {
    unsigned int period = getPeriodLength(i);
    // count number of odd lengths (if not a perfect square)
    if (period % 2 == 1)
      numOdd++;
    // branchless: numOdd += period & 1;
  }

  // print result
  std::cout << numOdd << std::endl;

  return 0;
}
