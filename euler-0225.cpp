// ////////////////////////////////////////////////////////
// # Title
// Tribonacci non-divisors
//
// # URL
// https://projecteuler.net/problem=225
// http://euler.stephan-brumme.com/225/
//
// # Problem
// The sequence 1, 1, 1, 3, 5, 9, 17, 31, 57, 105, 193, 355, 653, 1201 ...
// is defined by `T_1 = T_2 = T_3 = 1` and `T_n = T_{n-1} + T_{n-2} + T_{n-3}`.
//
// It can be shown that 27 does not divide any terms of this sequence.
// In fact, 27 is the first odd number with this property.
//
// Find the 124th odd number that does not divide any terms of the above sequence.
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// Computing a new element of the Tribonacci sequence requires the last three elements.
// They are called ''tri1'', ''tri2'' and ''tri3'' in my code, the next element is ''triNext'':
// ''unsigned int triNext = tri1 + tri2 + tri3;''
//
// My divisibility check computes a number's modulo and aborts if it becomes zero (''current'' is the current divisor, e.g. 27):
// ''triNext %= current;''
// ''if (triNext == 0)''
// ''  break;''
//
// Unfortunately, Tribonacci numbers get massive after just a few steps. Luckily:
// `(x + y) mod m = (x mod m + y mod m) mod m`
//
// Hence my variables ''tri1'', ''tri2'', ''tri3'' and ''triNext'' don't have to store true Tribonacci numbers but only the remainder modulo the current divisor.
//
// To find out for sure that a number is a non-divisor you have to find a cycle, that means that the remainders repeat themselves.
// There are a few smart ways to do that in a 100% safe way - but I decided to have a fixed number of steps and assume a number is a non-divisor if it passed all tests.
// The only optimization in my code is to detect whether ''tri1 == tri2 == tri3 == 1'' because then it returned to the starting point and there is a loop.
// (side note: that simple optimization is sufficient to reliably find the 124th number, though, and I'm not entirely sure why).
// My prototype found that 1577 is the divisor where my program needed 22710 steps to prove that it divides a Tribonacci number.
// That's why ''maxSteps = 22710''.
//
// # Note
// It seems `divisor^2` is a hard limit for ''MaxSteps''. My empirically found value is much smaller for most Tribonacci numbers and therefore faster.
//
// The modulo operation ''triNext %= current;'' can be slow.
// ''tri1'', ''tri2'' and ''tri3'' are each smaller than ''current'', therefore their sum ''triNext'' must be smaller than ''3*current'', too.
// The following code is about twice as fast as the modulo operation (which I kept in my solution):
// ''while (triNext >= current)''
// ''  triNext -= current;''
//
// I thought that a fully unrolled loop could be even faster, but it isn't:
// ''if (triNext >= current)''
// ''{''
// ''  triNext -= current;''
// ''  if (triNext >= current)''
// ''    triNext -= current;''
// ''}''

#include <iostream>

int main()
{
  // look for 124th number
  unsigned int index = 124;
  std::cin >> index;

  // 22710 steps until 1577 is a divisor (worst number I observed until the result is found)
  const unsigned int MaxSteps = 22710;

  // current divisor
  unsigned int current  = 1; // yes, I could start at 29, too

  // found non-divisors
  unsigned int numFound = 0;
  while (numFound < index)
  {
    // check next divisor
    current += 2;

    // first three numbers of the sequence
    unsigned int tri1 = 1;
    unsigned int tri2 = 1;
    unsigned int tri3 = 1;

    bool isDivisor = false;
    for (unsigned int step = 0; step <= MaxSteps; step++)
    {
      // next element in sequence
      unsigned int triNext = tri1 + tri2 + tri3;
      // I need only its remainder
      triNext %= current;

      // no remainder ? => divisor
      if (triNext == 0)
      {
        isDivisor = true;
        break;
      }

      // update last three elements of the sequence
      tri1 = tri2;
      tri2 = tri3;
      tri3 = triNext;

      // returned to original state ? must be a loop
      if (tri1 == 1 && tri2 == 1 && tri3 == 1)
        break;
    }

    // found a new non-divisor
    if (!isDivisor)
      numFound++;
  }

  // display last non-divisor
  std::cout << current << std::endl;
  return 0;
}
