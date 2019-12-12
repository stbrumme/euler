// ////////////////////////////////////////////////////////
// # Title
// Hilbert's New Hotel
//
// # URL
// https://projecteuler.net/problem=359
// http://euler.stephan-brumme.com/359/
//
// # Problem
// An infinite number of people (numbered 1, 2, 3, etc.) are lined up to get a room at Hilbert's newest infinite hotel.
// The hotel contains an infinite number of floors (numbered 1, 2, 3, etc.), and each floor contains an infinite number of rooms (numbered 1, 2, 3, etc.).
//
// Initially the hotel is empty. Hilbert declares a rule on how the nth person is assigned a room:
// person `n` gets the first vacant room in the lowest numbered floor satisfying either of the following:
//
// - the floor is empty
// - the floor is not empty, and if the latest person taking a room in that floor is person `m`, then `m + n` is a perfect square
//
// Person 1 gets room 1 in floor 1 since floor 1 is empty.
// Person 2 does not get room 2 in floor 1 since 1 + 2 = 3 is not a perfect square.
// Person 2 instead gets room 1 in floor 2 since floor 2 is empty.
// Person 3 gets room 2 in floor 1 since 1 + 3 = 4 is a perfect square.
//
// Eventually, every person in the line gets a room in the hotel.
//
// Define `P(f, r)` to be `n` if person n occupies room `r` in floor `f`, and 0 if no person occupies the room. Here are a few examples:
// `P(1, 1) = 1`
// `P(1, 2) = 3`
// `P(2, 1) = 2`
// `P(10, 20) = 440`
// `P(25, 75) = 4863`
// `P(99, 100) = 19454`
//
// Find the sum of all `P(f, r)` for all positive `f` and `r` such that `f * r = 71328803586048` and give the last 8 digits as your answer.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// My standard procedure is to make sure I can solve the problem for small input values:
// - ''fillHotel()'' "puts" guests into their ''rooms''
// - ''Pslow()'' performs a simple lookup in ''rooms''
// That's pretty fast for a few thousands guests but there's no chance of solving the real problem.
//
// Printing the guest IDs of the first floors / rooms showed a certain pattern:
// ||        8      ||  3  ||  3  ||  3  ||  3  ||  3  ||  3  ||  3  ||  3  ||  3  ||
// ||! floor \ room ++  1  ++  2  ++  3  ++  4  ++  5  ++  6  ++  7  ++  8  ++  9  ||
// ||        1      ++  1  ++  3  ++  6  ++ 10  ++ 15  ++ 21  ++ 28  ++ 36  ++ 45  ||
// ||        2      ++  2  ++  7  ++  9  ++ 16  ++ 20  ++ 29  ++ 35  ++ 46  ++ 54  ||
// ||        3      ++  4  ++  5  ++ 11  ++ 14  ++ 22  ++ 27  ++ 37  ++ 44  ++ 56  ||
// ||        4      ++  8  ++ 17  ++ 19  ++ 30  ++ 34  ++ 47  ++ 53  ++ 68  ++ 76  ||
// ||        5      ++ 12  ++ 13  ++ 23  ++ 26  ++ 38  ++ 43  ++ 57  ++ 64  ++ 80  ||
// ||        6      ++ 18  ++ 31  ++ 33  ++ 48  ++ 52  ++ 69  ++ 75  ++ 94  ++ 102 ||
// ||        7      ++ 24  ++ 25  ++ 39  ++ 42  ++ 58  ++ 63  ++ 81  ++ 88  ++ 108 ||
// ||        8      ++ 32  ++ 49  ++ 51  ++ 70  ++ 74  ++ 95  ++ 101 ++ 124 ++ 132 ||
// ||        9      ++ 40  ++ 41  ++ 59  ++ 62  ++ 82  ++ 87  ++ 109 ++ 116 ++ 140 ||
//
// - the first floor is the triangular series `n(n+1)/2`
// - the difference between the first and the second guest is 1 on odd floors and `2f+1` on even floors
// - the difference between the second and the third guest is 2 on odd floors and `2f` on even floors
// - differences between odd  rooms on the second floor is 9-2=7, 20-9=11, 35-20=15, 54-35=19, ... ==> difference increases by 4 with each step
// - differences between even rooms on the second floor is 16-7=9, 29-16=13, 46-29=17, ... ==> difference increases by 4 with each step
// - the third, fourth, fifth, ... floor show a pattern similar to the pattern I found in the second floor
//
// There are five tasks:
// 1. be able to compute the first floor `P(1, r)`
// 2. find the first guest of a floor `P(f, 1)`
// 3. find the increment between first and second room `P(f, 2) - P(f, 1)`
// 4. find the increment between second and third room `P(f, 3) - P(f, 2)`
// 5. Based on 3. and 4. find the initial differences between odd rooms `P(f, 3) - P(f, 1)` and even rooms `P(f, 4) - P(f, 2)`
//
// With lots of trial-and-error I found that the first guest of floor `f` is
// (1) even-numbered floors: `P(f, 1) = \lfloor dfrac{floor^2}{2} \rfloor`
//
// (2) odd-numbered floors:  `P(f, 1) = dfrac{(floor + 1) * (floor - 1)}{2}` except for `P(1, 1) = 1`
//
// C++ automatically truncates quotients so ''(floor + 1) / 2 * floor'' is good enough for (1) while also helpful for (2).
//
// ''incrementOdd'' and ''incrementEven'' are set according to the rules I defined above.
// Unfortunately, a simple loop would take forever for large room numbers.
// However, the constant increase of 4 per step lets me use the triangle formula `dfrac{x(x+1)}{2}` with some modifications.
//
// The input value 71328803586048 has only two prime factors: `2^27 * 3^12`.
// Iterating over each combination `2^i * 3^j` where `0 <= i <= 27` and `0 <= j <= 12` returns all 364 divisors.
//
// # Note
// Well, this is one of those problems where I ask myself: whyyyyyyy ?
// I couldn't find a scientific approach and everything I did was playing around with weird formulas until ''P()'' produced the same output as ''Pslow()''.
// I didn't learn anything new nor did I have fun.
// It's kind of strange that so many people solved this problem (with ease ?!) because I'm pretty sure I saw a few patterns just by pure luck.
//
// Please keep in mind that I had to use G++'s 128 bit integer extension and therefore the code doesn't compile with Visual C++.

#include <iostream>
#include <vector>
#include <cmath>

// ---------- slow algorithm ----------

std::vector<std::vector<unsigned int>> rooms;

// look up result in precomputed 2D vector "rooms"
unsigned int Pslow(unsigned int floor, unsigned int room)
{
  // from one-indexed to zero-based
  floor--;
  room--;

  if (floor >= rooms.size())
    return 0;
  if (room >= rooms[floor].size())
    return 0;

  return rooms[floor][room];
}

// return true if x is a perfect square
bool isSquare(unsigned int x)
{
  unsigned int root = sqrt(x);
  return root * root == x;
}

// precompute Hilbert's New Hotel, too slow for the problem, but solves the examples
void fillHotel(unsigned int numPersons)
{
  // one iteration per person
  for (unsigned int person = 1; person < numPersons; person++)
  {
    bool needNewFloor = true;
    // try to place person in an existing floor
    for (size_t floor = 0; floor < rooms.size(); floor++)
    {
      // last + person should be a perfect square
      if (isSquare(rooms[floor].back() + person))
      {
        rooms[floor].push_back(person);
        needNewFloor = false;
        break;
      }
    }

    // no suitable floor found, create a new floor
    if (needNewFloor)
    {
      std::vector<unsigned int> newFloor = { person };
      rooms.push_back(newFloor);
    }
  }
}

// ---------- fast algorithm ----------

// compute P(floor, room)
unsigned int P(unsigned long long floor, unsigned long long room, unsigned int modulo = 100000000)
{
  // compute number in first room of that floor
  __int128 result = (__int128)(floor + 1) / 2 * floor;
  if (floor % 2 == 1 && floor > 1)
    result -= (floor + 1) / 2;

  // separate increments for odd and even rooms
  __int128 incrementEven = 1;
  if (floor % 2 == 0)
    incrementEven = 2 * floor + 1;

  __int128 incrementOdd = 2;
  if (floor % 2 == 1)
    incrementOdd = 2 * floor;

  // and they are a bit different on the first floor
  if (floor == 1)
  {
    incrementOdd  = 3;
    incrementEven = 2;
  }

  // my original code:
  //for (unsigned int i = 2; i <= room; i += 2)
  //{
  //  result += incrementEven;
  //  incrementEven += 2;
  //}
  //for (unsigned int i = 3; i <= room; i += 2)
  //{
  //  result += incrementOdd;
  //  incrementOdd += 2;
  //}
  // and converted to a closed form:

  // number of rooms with even room numbers
  __int128 numEven = room / 2;
  // sum of 1+2+3+...+numEven
  auto triangleEven = numEven * (numEven + 1) / 2;
  // sum of 2+4+6+...+2*numEven
  triangleEven *= 2;

  // number of rooms with even odd numbers
  __int128 numOdd = (room - 1) / 2;
  // sum of 1+2+3+...+numOdd
  auto triangleOdd = numOdd * (numOdd + 1) / 2;
  // sum of 2+4+6+...+2*numOdd
  triangleOdd *= 2;

  result += numEven * (incrementEven - 2) + triangleEven;
  result += numOdd  * (incrementOdd  - 2) + triangleOdd;

  return result % modulo;
}

int main()
{
  auto maxExponentTwo   = 27;
  auto maxExponentThree = 12;
  auto number = 71328803586048ULL;

  std::cin >> maxExponentTwo >> maxExponentThree;
  number = pow(2ULL, maxExponentTwo) * pow(3ULL, maxExponentThree);

  const unsigned int Modulo = 100000000;

  // solve examples with slow algorithm
  //fillHotel(20000);
  //std::cout << Pslow( 1,  1) << " " << Pslow( 1,  2) << " " << Pslow( 2,   1) << " "
  //          << Pslow(10, 20) << " " << Pslow(25, 75) << " " << Pslow(99, 100) << std::endl;

  unsigned int sum = 0;
  // 71328803586048 = 2^27 * 3^12
  auto two = 1ULL;
  // iterate over all exponents of 2 and 3
  for (auto expTwo = 0; expTwo <= maxExponentTwo; expTwo++, two *= 2)
  {
    auto three = 1ULL;
    for (auto expThree = 0; expThree <= maxExponentThree; expThree++, three *= 3)
    {
      auto floor = two * three;    //         2^expTwo      * 3^expThree
      auto room  = number / floor; // will be 2^(27-expTwo) * 3^(12-expThree)
      sum += P(floor, room, Modulo);
      sum %= Modulo;
    }
  }

  // finally ...
  std::cout << sum << std::endl;
  return 0;
}
