// ////////////////////////////////////////////////////////
// # Title
// Quadtree encoding (a simple compression algorithm)
//
// # URL
// https://projecteuler.net/problem=287
// http://euler.stephan-brumme.com/287/
//
// # Problem
// The quadtree encoding allows us to describe a `2^N * 2^N` black and white image as a sequence of bits (0 and 1).
// Those sequences are to be read from left to right like this:
// - the first bit deals with the complete `2^N * 2^N` region;
// - "0" denotes a split: the current `2^n * 2^n` region is divided into 4 sub-regions of dimension `2^{n-1} * 2^{n-1}`,
//                        the next bits contains the description of the top left, top right, bottom left and bottom right sub-regions - in that order;
// - "10" indicates that the current region contains only black pixels;
// - "11" indicates that the current region contains only white pixels.
//
// Consider the following 4x4 image (colored marks denote places where a split can occur):
//
// ![image](quadtree.gif)
//
// This image can be described by several sequences, for example : "001010101001011111011010101010", of length 30, or
// "0100101111101110", of length 16, which is the minimal sequence for this image.
//
// For a positive integer `N`, define `D_N` as the `2^N * 2^N` image with the following coloring scheme:
//
//    the pixel with coordinates x = 0, y = 0 corresponds to the bottom left pixel,
//    if `(x - 2^{N-1})^2 + (y - 2^{N-1})^2 <= 2^{2N-2}` then the pixel is black,
//    otherwise the pixel is white.
//
// What is the length of the minimal sequence describing `D_24` ?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// The given formula can be easily translated to a function ''isBlack(x,y)'' that returns ''true'' if the pixel at (x,y) is black.
// It looks like the equation of a circle - and when printing `D_4` on screen I get (see ''#ifdef DRAW_IMAGE''):
// ''.....BBBBBBB....''
// ''...BBBBBBBBBBB..''
// ''..BBBBBBBBBBBBB.''
// ''..BBBBBBBBBBBBB.''
// ''.BBBBBBBBBBBBBBB''
// ''.BBBBBBBBBBBBBBB''
// ''.BBBBBBBBBBBBBBB''
// ''BBBBBBBBBBBBBBBB''
// ''.BBBBBBBBBBBBBBB''
// ''.BBBBBBBBBBBBBBB''
// ''.BBBBBBBBBBBBBBB''
// ''..BBBBBBBBBBBBB.''
// ''..BBBBBBBBBBBBB.''
// ''...BBBBBBBBBBB..''
// ''.....BBBBBBB....''
// ''........B.......''
//
// The combination of a circle and rectangles has some nice properties:
// - if all four corners of the rectangle are inside the circle (= black) then the whole rectangle is black
// - if the rectangle is much smaller than the circle and all fours corners of the rectangle are outside the circle then the whole rectangle is outside the circle
//
// My function ''encode'' returns the size of an optimal encoding and performs the following tasks in a recursive manner:
// - if the current square covers only 1 pixel then it needs 2 bits
// - check all four corners of the current square, if they are all inside or outside then its encoding requires 2 bits
// - subdivide the current square into 4 equally-sized squares and determine their encoding size plus 1 bit for the split
//
// # Note
// When the current square is 2x2 and some of its corner are black and some are white then I know that these four pixels need 1 + 2+2+2+2 bits, saving one recursion step (about 10% faster).
//
// The encoded image has a compression ratio of about 900000 to 1. That's excellent - photos usually compress to about 10:1.

#include <iostream>

// D24 => 2^24
unsigned int size = 1 << 24;

// return true if pixel at (x,y) is black
bool isBlack(unsigned int x, unsigned int y)
{
  // 2^(N-1)
  long long middle = size >> 1;
  // right side of the equation: 2^(2N - 2) = 2^(N-1) * 2^(N-1) = middle * middle
  auto threshold = middle * middle;

  // be a bit careful with negative differences
  auto dx = (long long)x - middle;
  auto dy = (long long)y - middle;
  return dx*dx + dy*dy <= threshold;
}

// return size of optimal encoding
// note: I expect only valid input, such that (toX - fromX) = (toY - fromY) = a power of two
unsigned int encode(unsigned int fromX, unsigned int fromY, unsigned int toX, unsigned int toY, bool isFirst = true)
{
  // a single pixel ?
  if (fromX == toX) // implies fromY == toY
    return 2; // doesn't matter whether black or white, both encodings need two bits

  // isBlack() will produce a black circle
  // checking all four corners is sufficient to know when to split
  bool a = isBlack(fromX, fromY);
  bool b = isBlack(toX,   fromY);
  bool c = isBlack(toX,   toY);
  bool d = isBlack(fromX, toY);

  // same color on all four corner => the whole area is covered by the same color
  // however, this assumption doesn't hold on the first level
  if (a == b && b == c && c == d && !isFirst)
    return 2; // again: the color doesn't matter, both need two bits to fill the entire area

  // speed optimization: if a 2x2 area needs to be split, then it always requires 9 bits
  if (fromX + 1 == toX)
    return 1 + 4*2; // a split marker and four single pixels (2 bits each)

  // split evenly
  auto half = (toX - fromX + 1) / 2;
  return encode(fromX,        fromY + half, toX - half, toY,        false) + // upper-left
         encode(fromX + half, fromY + half, toX,        toY,        false) + // upper-right
         encode(fromX,        fromY,        toX - half, toY - half, false) + // lower-left
         encode(fromX + half, fromY,        toX,        toY - half, false) + // lower-right
         1; // don't forget: there's one bit for the split marker
}

int main()
{
  // D24 => 24
  unsigned int shift = 4;
  std::cin >> shift;

  // length of an edge of the image in pixels
  size = 1 << shift;

  // draw on screen (only useful for very small values of "shift" 1 ... 5)
#define DRAW_IMAGE
#ifdef DRAW_IMAGE
  if (size <= 5)
  {
    for (unsigned int y = 0; y < size; y++)
    {
      auto flipY = (size - 1) - y; // problem states that lower-left corner is (0,0), must flip image upside-down
      for (unsigned int x = 0; x < size; x++)
        std::cout << (isBlack(x, flipY) ? "B" : ".");
      std::cout << std::endl;
    }
  }
#endif

  // let's compress to infinity and beyond !
  std::cout << encode(0, 0, size - 1, size - 1) << std::endl;
  return 0;
}
