// ////////////////////////////////////////////////////////
// # Title
// Minimal network
//
// # URL
// https://projecteuler.net/problem=107
// http://euler.stephan-brumme.com/107/
//
// # Problem
// The following undirected network consists of seven vertices and twelve edges with a total weight of 243.
//
// ![full network](p107_1.gif)
//
// The same network can be represented by the matrix below.
//
// || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 ||
// ||   ++ A ++ B ++ C ++ D ++ E ++ F ++ G ||
// || A ++   ++ 16++ 12++ 21++   ++   ++   ||
// || B ++ 16++   ++   ++ 17++ 20++   ++   ||
// || C ++ 12++   ++   ++ 28++   ++ 31++   ||
// || D ++ 21++ 17++ 28++   ++ 18++ 19++ 23||
// || E ++   ++ 20++   ++ 18++   ++   ++ 11||
// || F ++   ++   ++ 31++ 19++   ++   ++ 27||
// || G ++   ++   ++   ++ 23++ 11++ 27++   ||
//
// However, it is possible to optimise the network by removing some edges and still ensure that all points on the network remain connected.
// The network which achieves the maximum saving is shown below. It has a weight of 93, representing a saving of 243 - 93 = 150 from the original network.
//
// ![optimized network](p107_2.gif)
//
// Using [network.txt](https://projecteuler.net/project/resources/p107_network.txt) (right click and 'Save Link/Target As...'), a 6K text file containing a network with forty vertices,
// and given in matrix form, find the maximum saving which can be achieved by removing redundant edges whilst ensuring that the network remains connected.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// I use Prim's algorithm (see https://en.wikipedia.org/wiki/Prim%27s_algorithm ):
// - ''done'' contains these nodes where all connections are optimized
// - ''done'' contains initially an arbitrarily chosen node
// - algorithm is finished when ''done'' contains all nodes
// - in each iteration, one node is added to ''done''
//
// One iteration performs these tasks:
// - store all edges in ''next'' where one node is in ''done'' and the other node isn't
// - sort ''next'' by the weight
// - pick the lowest
// - add that edge to the optimized graph ''minimal''
// - add the other edge's node to ''done'', too
//
// There will be 40-1=39 iterations. The most time-consuming part is locating all potential edges.
// The "true" algorithm of Prim uses a priority queue quite efficiently whereas I rebuild it from scratch every time.
// Nevertheless, my code solves the 2500x2500 networks of Hackerrank within less than a second.
//
// # Hackerrank
// - the input format is completely different
// - two nodes may be connected via multiple edges
// - print the optimized weight instead of the gain

//#define ORIGINAL

#include <iostream>

#include <set>
#include <map>
#include <queue>

typedef unsigned int Node;
typedef unsigned int Weight;

// connect two nodes and assign a weight
struct Edge
{
  Node from;
  Node to;
  Weight weight;

  // for sorting
  bool operator<(const Edge& other) const
  {
    // STL's priority queue returns the __largest__ element first, therefore I invert the comparison sign
    if (weight != other.weight)
      return weight > other.weight;

    // sort by nodes, too, to avoid collisions of nodes with the same weight
    // the order doesn't matter, these nodes just "have to be different"
    if (from   != other.from)
      return from   < other.from;

    return to < other.to;
  }
};

// all edges (full network)
std::map<std::pair<Node, Node>, Weight> network;
// all nodes
std::set<Node> nodes;
// weight of the full network, including duplicate connections
unsigned int initialSum = 0;

// insert an edge into the network, true if already existing
void addEdge(Node from, Node to, Weight weight)
{
  // count all originals weights
  initialSum += weight;

  // all edges have the lower ID first (I can do that because it's an undirected graph)
  if (from > to)
    std::swap(from, to);

  // already existing edge ?
  auto fromTo = std::make_pair(from, to);
  if (network.count(fromTo) != 0)
    // has the existing edge a lower or the same weight ? => we're done
    if (network[fromTo] <= weight)
      return;

  // store new (or improved) connection between two nodes
  network[fromTo] = weight;
  nodes.insert(from);
  nodes.insert(to);
}

int main()
{
#ifdef ORIGINAL
  // read Project Euler's weird file format
  for (unsigned int i = 0; i < 40; i++)
    for (unsigned int j = 0; j < 40; j++)
    {
      char c = 0;
      unsigned int weight = 0;
      // CSV-format
      while (c != ',' && c != '\n')
      {
        c = std::cin.get();
        // ignore dashes
        if (c >= '0' && c <= '9')
        {
          weight *= 10;
          weight += c - '0';
        }
      }

      // only valid weights
      if (weight != 0 && i < j)
        addEdge(i, j, weight);
    }

#else

  // read Hackerrank input
  unsigned int numNodes, numEdges;
  std::cin >> numNodes >> numEdges;
  for (unsigned int i = 0; i < numEdges; i++)
  {
    Node   from, to;
    Weight weight;
    std::cin >> from >> to >> weight;
    addEdge(from, to, weight);
  }
#endif

  // optimized graph, initially empty
  std::set<Edge> minimal;

  // start with a random node
  std::set<Node> done;
  done.insert(*nodes.begin());

  // not all nodes optimized yet ?
  while (done.size() < nodes.size())
  {
    // add all edges where one node is part of tree and the other isn't
    std::priority_queue<Edge> next;
    for (auto e : network)
    {
      auto fromTo  = e.first;
      bool hasFrom = done.count(fromTo.first)  != 0;
      bool hasTo   = done.count(fromTo.second) != 0;
      if (hasFrom == hasTo) // both nodes are already optimized or both aren't ?
        continue;

      next.push({fromTo.first, fromTo.second, e.second});
    }

    // get edge with minimal weight
    auto add = next.top();

    // add new edge to tree
    done.insert(add.from); // one insert is redundant but set::set throws it away
    done.insert(add.to);

    // add edge to the optimized graph
    minimal.insert(add);
  }

  // "measure" both graphs
  unsigned int optimizedSum = 0;
  for (auto i : minimal)
    optimizedSum += i.weight;

#ifdef ORIGINAL
  // difference
  auto gain = initialSum - optimizedSum;
  std::cout << gain << std::endl;
#else
  std::cout << optimizedSum << std::endl;
#endif

  return 0;
}
