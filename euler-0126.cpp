// ////////////////////////////////////////////////////////
// # Title
// Cuboid layers
//
// # URL
// https://projecteuler.net/problem=126
// http://euler.stephan-brumme.com/126/
//
// # Problem
// The minimum number of cubes to cover every visible face on a cuboid measuring 3 x 2 x 1 is twenty-two.
//
// ![cuboids](p126.gif)
//
// If we then add a second layer to this solid it would require forty-six cubes to cover every visible face,
// the third layer would require seventy-eight cubes, and the fourth layer would require one-hundred and eighteen cubes to cover every visible face.
//
// However, the first layer on a cuboid measuring 5 x 1 x 1 also requires twenty-two cubes; similarly the first layer on cuboids measuring 5 x 3 x 1, 7 x 2 x 1, and 11 x 1 x 1 all contain forty-six cubes.
//
// We shall define `C(n)` to represent the number of cuboids that contain n cubes in one of its layers. So `C(22) = 2`, `C(46) = 4`, `C(78) = 5`, and `C(118) = 8`.
//
// It turns out that 154 is the least value of n for which `C(n) = 10`.
//
// Find the least value of n for which `C(n) = 1000`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// The initial cuboid (without any layers) has six sides:
// - front and back side, both with an area of `width * height`
// - left and right side, both with an area of `height * depth`
// - top and bottom side, both with an area of `depth * width`
// The total surface area is `2 * (width * height + height * depth + depth * width)`.
// Each part of the surface is covered by the first layer. That means:
// `C(1) = 2 * (width * height + height * depth + depth * width)`.
//
// For the 3 x 2 x 1 cuboid: `C(1) = 2 * (3 * 2 + 2 * 1 + 1 * 3) = 2 * (6 + 2 + 3) = 2 * 11 = 22`
//
// The computation of the second, third, fourth, ... layer wasn't obvious to me. My drawing skills are sub-par at best.
// So I decided to write some code: start with a shape and move that shape one unit left, right, up, down, forward and backward
// (start each move at the initial position). The difference between the old shape and the shape created by these six movements is the volume of the new layer.
// My function ''naive()'' (last lines of my code) performs exactly these steps. Starting with a specified cuboid it finds the volume of each additional layer and prints it.
//
// Playing around with several cuboid sizes I saw a pattern:
// `C(2) = C(1) + 4 * (width + height + depth)`
// `C(3) = C(2) + 4 * (width + height + depth) + 8`
// `C(4) = C(3) + 4 * (width + height + depth) + 16`
// `C(5) = C(4) + 4 * (width + height + depth) + 24`
//
// A bit more general:
// `C(1) = 2 * (width * height + height * depth + depth * width)`.
// `C(n) = C(n-1) + 4 * (width + height + depth) + 8 * (n - 2)`
//
// You can convert the second formula into a closed form but then the equation looks a bit messy.
// The C++ compiler won't generate much faster code, too. The function ''Cuboid::calculate'' evaluates these two equations.
//
// In order to solve `C(n) = x` for arbitrary `x` (it's `x = 1000` in the original problem) I wrote code based on ''std::map'':
// - each cuboids is represented by ''Cuboid'', it stores the size of the initial cuboid and the number of layers
// - ''Cuboid'' can be sorted by the size of the outermost layer (and then by it's sides)
// - since ''std::map'' is an ascendingly sorted container, it's first element is the cuboid with the smallest outermost layer
//
// Starting with the most basic cuboid (1,1,1) with only 1 layer I repeat the following:
// - pick and remove the first (= smallest) cuboid from the container
// - add a cuboid with ''width + 1'' to the container
// - add another cuboid with ''height + 1''
// - and a cuboid with ''depth + 1''
// - finally add a cuboid with ''layer + 1'' to that container
//
// Adding a cuboid is encapsulated in the ''add()'' function. It rejects cuboids where ''height > width'' and ''depth > height'' to avoid
// having the same (rotated) cuboid multiple times in the container. Adding exactly the same container twice is prevented, too.
// On top of that, ''add()'' keeps track of the volume of the most recent layer (in ''count[]'').
//
// That algorithm stops when the smallest cuboid has exactly 1000 "siblings" (==> the ''count'' of the current volume is 1000).
// It shows the correct result after about 4 seconds.
//
// However, my profiler showed that a few million cuboids where added (and removed) from the container.
// The overhead of memory allocation was responsible for the majority of the execution time.
//
// A much faster approach is to guess an upper limit of the result and then create all cuboids (and layers) that fit into that limit.
// Keeping track of just the various values of `C()` instead of the cuboids can be done with a small ''std::vector''.
// The program becomes about 50x faster and has almost no memory consumption. See my function ''fast()''.
//
// The main problem is to find a good guess for the upper limit.
// ''fast()'' returns 0 if no result was found, that's when the initial guess has to be incremented and another call to ''fast()'' is required.
// Starting with a very high guess will find the result but might waste lots of time computing cuboids with too large volumes.
// Currently, I use a ''StepSize = 10000'' ... yes, I know it's not optimal (it doesn't find the result in its first iteration)
// but I deliberately keep it this way.
// If ''StepSize = 50'' then ''fast()'' isn't faster than my original code.
// If ''StepSize = 150000'' then ''fast()'' isn't faster than my original code.
// That means for the original problem, ''fast()'' is only truly faster if ''50 < StepSize < 150000''.
//
// # Note
// Even though the code of ''fast()'' is shorter (and can be faster), I have a strange feeling about it:
// somehow it needs a good guess but I haven't found a way to come up with a straightforward way to generate that guess.
//
// I was stuck for a while because initially I stopped when I found a layer volume with __at least__ 1000 matching cuboids (instead of __exactly__ 1000).
// There are a few volumes below the correct solution with more than 1000 cuboids.
//
// Kristian Edlund visualized a nice way to enumerate the cubes added in each layer. His blog can found in my [links](#links).

#include <iostream>
#include <vector>
#include <set>
#include <map>

// a cuboid with 1+ added layers
struct Cuboid
{
  // cuboid size
  const unsigned short width;
  const unsigned short height;
  const unsigned short depth;
  // wrapped several times
  const unsigned short layers;
  // compute volume with calculated()
  const unsigned int   hullVolume;

  // create new cuboid
  Cuboid(unsigned short width_, unsigned short height_, unsigned short depth_, unsigned short layers_)
  : width(width_), height(height_), depth(depth_), layers(layers_),
    hullVolume(calculate())
  {}

  // calculate volume of the most recent / outermost layer
  unsigned int calculate() const
  {
    // innermost hull
    auto result = 2 * ((unsigned int)width  * height +
                       (unsigned int)height * depth  +
                       (unsigned int)depth  * width);

    // need just one layer ?
    if (layers == 1)
      return result;

    // add more layers
    auto increment = 4 * (width + height + depth);
    for (unsigned short current = 2; current <= layers; current++)
    {
      result    += increment;
      increment += 8;
    }

    return result;
  }

  // for std::set
  bool operator<(const Cuboid& other) const
  {
    // volume
    if (hullVolume < other.hullVolume) return true;
    if (hullVolume > other.hullVolume) return false;
    // the following lines are arbitrary => they only need to be consistent
    // sides
    if (width  < other.width)  return true;
    if (width  > other.width)  return false;
    if (height < other.height) return true;
    if (height > other.height) return false;
    if (depth  < other.depth)  return true;
    if (depth  > other.depth)  return false;
    // layers
    return layers < other.layers;
  }
};

// list of cuboids, sorted by volume of their outermost layer
std::set<Cuboid> todo;
// count how many cuboid layers have a certain volume (looking for 1000)
std::map<unsigned int, unsigned int> count; // could be replaced by std::vector if you can estimate the upper limit
// note: this information is implicitely stored in "todo" as well: count the consecutive cuboids with the same hullVolume

// add a cuboid with a certain amount of layers, return number of cuboids with the same hull volume
bool add(unsigned short width, unsigned short height, unsigned short depth, unsigned short layers)
{
  // accept only width >= height >= depth
  if (width  < height)
    return false;
  if (height < depth)
    return false;

  // create new cuboid
  Cuboid cuboid(width, height, depth, layers);

  // already queued ?
  if (todo.count(cuboid) != 0)
    return false;

  // add cuboid
  todo.insert(cuboid);
  count[cuboid.hullVolume]++;

  // successfully added
  return true;
}

const unsigned int Failed = 0;
// find the correct result if you know an upper bound maxVolume, if not found return Failed
unsigned int fast(unsigned int stopIf, unsigned int maxVolume)
{
  // allocate enough memory
  std::vector<unsigned int> count(maxVolume + 1, 0);

  // just four nested loops to generate all cuboids with hulls <= maxVolume
  // ensure that x >= y >= z to avoid enumerating some volume multiple times
  for (unsigned short x = 1; Cuboid(x,1,1,1).hullVolume <= maxVolume; x++)
    for (unsigned short y = 1; y <= x && Cuboid(x,y,1,1).hullVolume <= maxVolume; y++)
      for (unsigned short z = 1; z <= y && Cuboid(x,y,z,1).hullVolume <= maxVolume; z++)
        for (unsigned short layer = 1; ; layer++)
        {
          auto current = Cuboid(x,y,z,layer).hullVolume;
          if (current > maxVolume)
            break;

          count[current]++;
        }

  // find smallest hull volume that appears exactly 1000 times
  for (size_t i = 0; i < count.size(); i++)
    if (count[i] == stopIf)
      return i;

  // failed
  return Failed;
}

// forward declaration of my old helper code
//unsigned int naive(unsigned int x, unsigned int y, unsigned int z, unsigned int layers);

int main()
{
  // start old code to analyze a single cuboid
  //naive(3,2,1, 5); return 0;

  // look for that number of cuboids with the same volume
  unsigned int stopIf = 1000;
  std::cin >> stopIf;

  // ---------- fast enumeration ----------

  // find solution if an upper limit of the result is known
#define FAST_ENUMERATION
#ifdef  FAST_ENUMERATION
  const unsigned int StepSize = 10000; // I deliberately choose not the optimal value !
  // assume that the result is below 10000, then 20000, ... until fast() returns > 0
  for (unsigned int maxVolume = StepSize; ; maxVolume += StepSize)
  {
    auto current = fast(stopIf, maxVolume); // returns 0 if not found
    // found ?
    if (current != Failed)
    {
      std::cout << current << std::endl;
      return 0;
    }
  }
#endif

  // ---------- slower enumeration ----------

  // note: I found the correct result with this approach but later created the faster enumeration you find above

  // initial cuboid: one unit wide, high and deep with one layer around it
  add(1,1,1,1);
  unsigned int volume = 0;
  while (count[volume] != stopIf)
  {
    // pick cuboid with smallest volume and remove it
    auto current = *(todo.begin());
    todo.erase(todo.begin());

    volume = current.hullVolume;

    // enlarge cuboid
    add(current.width + 1, current.height, current.depth, current.layers);
    add(current.width, current.height + 1, current.depth, current.layers);
    add(current.width, current.height, current.depth + 1, current.layers);
    // add one layer
    add(current.width, current.height, current.depth, current.layers + 1);
  }

  // display result
  std::cout << volume << std::endl;
  return 0;
}

// ---------- compute hull volume, slow approach ----------

// a single cube that is part of an cuboid (more or less a coordinate)
struct Cube
{
  // coordinates
  const int x, y, z;

  // create new cube
  Cube(int xx, int yy, int zz)
  : x(xx), y(yy), z(zz)
  {}

  // for std::set
  bool operator<(const Cube& other) const
  {
    if (x < other.x) return true;
    if (x > other.x) return false;
    if (y < other.y) return true;
    if (y > other.y) return false;
    return z < other.z;
  }
};

unsigned int naive(unsigned int x, unsigned int y, unsigned int z, unsigned int layers)
{
  // keep track of all cubes belonging to the shape
  std::set<Cube> cuboid;
  // cubes added in the most recent layer
  std::vector<Cube> hull;

  // initial shape
  for (int xx = 0; xx < (int)x; xx++)
    for (int yy = 0; yy < (int)y; yy++)
      for (int zz = 0; zz < (int)z; zz++)
      {
        Cube current(xx, yy, zz);
        cuboid.insert(current);
        hull.push_back(current);
      }

  // add desired numbers of layers
  for (unsigned int layer = 1; layer <= layers; layer++)
  {
    std::vector<Cube> next;
    for (auto current : hull)
    {
      // grow in each direction x,y,z
      Cube left (current.x - 1, current.y, current.z);
      Cube right(current.x + 1, current.y, current.z);
      Cube down (current.x, current.y - 1, current.z);
      Cube up   (current.x, current.y + 1, current.z);
      Cube back (current.x, current.y, current.z - 1);
      Cube front(current.x, current.y, current.z + 1);

      // add cube (if not already existing)
      if (cuboid.count(left)  == 0)
      {
        cuboid.insert(left);
        next.push_back(left);
      }
      if (cuboid.count(right) == 0)
      {
        cuboid.insert(right);
        next.push_back(right);
      }
      if (cuboid.count(down)  == 0)
      {
        cuboid.insert(down);
        next.push_back(down);
      }
      if (cuboid.count(up)    == 0)
      {
        cuboid.insert(up);
        next.push_back(up);
      }
      if (cuboid.count(back)  == 0)
      {
        cuboid.insert(back);
        next.push_back(back);
      }
      if (cuboid.count(front) == 0)
      {
        cuboid.insert(front);
        next.push_back(front);
      }
    }

    std::cout << "layer " << layer << ": "
              << "C(" << x << "," << y << "," << z << ")=" << next.size();
    if (layer > 1)
      std::cout << " delta=" << next.size() - hull.size();
    std::cout << std::endl;

    // next iteration
    hull = std::move(next);
  }

  return hull.size();
}
