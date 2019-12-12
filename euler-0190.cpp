// ////////////////////////////////////////////////////////
// # Title
// Maximising a weighted product
//
// # URL
// https://projecteuler.net/problem=190
// http://euler.stephan-brumme.com/190/
//
// # Problem
// Let `S_m = (x_1, x_2, ... , x_m)` be the m-tuple of positive real numbers with `x_1 + x_2 + ... + x_m = m` for which `P_m = x_1 * x_2^2 * ... * x_m^m` is maximised.
//
// For example, it can be verified that `\lfloor P_10 \rfloor = 4112` (`\lfloor \rfloor` is the integer part function).
//
// Find `sum{\lfloor Pm \rfloor}` for `2 <= m <= 15`.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I implemented a randomized search:
// 1. start with `x_1 = x_2 = ... = x_m = 1`
// 2. choose random positions `x_{from}` and `x_{to}`
// 3. subtract a small delta from `x_{from}`, add the same delta to `x_{to}`
// 4. evaluate `x_1 * x_2^2 * ... * x_m^m`, if it is bigger, keep the new `x_{from}` and `x_{to}`, else undo
// 5. go to step 2, unless the result didn't improve for a certain number of iterations
//
// My randomized search uses deltas 0.1, 0.01, 0.001, 0.0001, 0.00001 and 0.000001 (in that order) and aborts if no improvement
// has been observed in more than 100 iterations.
// You can probably reduce the number of deltas and/or iterations but these values were my initial "guess" and worked perfectly fine.
//
// In order to provide a reproducible result, I don't invoke the standard ''rand()'' function but provide my own simple ''myrand()''
// which always generates the same random numbers, no matter what compiler / standard library is installed on your computer.
//
// # Alternative
// After submitting my result I became aware that this kind of opimization problems can be solved with Lagrance multipliers, too (see https://en.wikipedia.org/wiki/Lagrange_multiplier ).

#include <iostream>
#include <vector>

// configure randomized search
const unsigned int StableIterations = 100;
const double       InitialDelta     =   0.1;
const double       FinalDelta       =   0.000001;

// a simple pseudo-random number generator, result in 0 .. modulo - 1
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand(unsigned int modulo)
{
  // code taken from problem 185
  static unsigned int seed = 0;
  seed = 1103515245 * seed + 12345;
  return (seed >> 16) % modulo; // modified: don't use low bits but start at bit 16
}

// randomly adjust values' elements by delta until no further change observed
double optimize(unsigned int numValues)
{
  // initially all values are 1
  std::vector<double> values(numValues, 1);

  double best = 0;
  for (double delta = InitialDelta; delta > FinalDelta; delta /= 10)
  {
    // abort if best value didn't change for a few iterations
    unsigned int countSame = 0;
    while (countSame < StableIterations)
    {
      countSame++;

      // select two random elements
      auto from = myrand(numValues);
      auto to   = myrand(numValues);
      // must be different from each other
      if (from == to)
        continue;
      // values[from] - delta must remain positive
      if (values[from] <= delta)
        continue;

      // adjust values: take delta from values[from] and move it to values[to]
      values[from] -= delta;
      values[to]   += delta;

      // compute new total value
      double current = 1;
      for (unsigned int j = 0; j < numValues; j++)
        // the following loop is the same as pow(values[j], j + 1)
        for (unsigned int exponent = 1; exponent <= j + 1; exponent++)
          current *= values[j];

      // higher than before ?
      if (best < current)
      {
        // yes, keep these values
        best = current;
        // reset counter
        countSame = 0;
      }
      else
      {
        // nope, restore old values
        values[from] += delta;
        values[to]   -= delta;
      }
    }
  }

  return best;
}

int main()
{
  unsigned int limit = 15;
  std::cin >> limit;

  unsigned int sum = 0;
  for (unsigned int m = 2; m <= limit; m++)
  {
    // optimize and round down
    sum += (unsigned int) optimize(m);

    // via Lagrance multipliers
    //double k = 2.0 / (m + 1);
    //double lagrange = 1;
    //for (unsigned int i = 1; i <= m; i++)
    //  lagrange *= pow(i * k, i);
  }

  // print result
  std::cout << sum << std::endl;
  return 0;
}
