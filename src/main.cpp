#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/enum.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/squared_distance_2.h>
#include <iterator>
#include <climits>
#include <boost/optional/optional_io.hpp>
#include "../include/utils.hpp"
#include "../include/polygonizationHelper.hpp"
#include "../include/algorithms.hpp"
#include <chrono>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef vector<Point_2> Points;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Triangle_2<K> Triangle_2; //?
typedef K::Segment_2 Segment_2;
typedef std::vector<Point_2>::iterator pveciterator;
typedef std::vector<Segment_2>::iterator sveciterator;
typedef Polygon_2::Edge_const_iterator edgeit;
typedef Polygon_2::Vertex_iterator vertexit;

using namespace std;
using namespace std::chrono;

const string EMPTY_STRING = "empty";

int main(int argc, char **argv)
{
  vector<string> commandLineArguments(argv, argv + argc);

  string inputFile = EMPTY_STRING;
  string outputFile = EMPTY_STRING;
  string algorithm = EMPTY_STRING;
  int edge_selection = 0;

  for (int i = 0; i < commandLineArguments.size(); i++)
  {
    if ((commandLineArguments[i] == "-i") && ((i + 1) < commandLineArguments.size()))
    {
      inputFile.assign(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-o") && ((i + 1) < commandLineArguments.size()))
    {
      outputFile.assign(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-algorithm") && ((i + 1) < commandLineArguments.size()))
    {
      algorithm.assign(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-edge_selection") && ((i + 1) < commandLineArguments.size()))
    {
      edge_selection = stoi(commandLineArguments[i + 1]);
    }
  }

  // error checking
  if (inputFile == EMPTY_STRING || outputFile == EMPTY_STRING || algorithm == EMPTY_STRING || edge_selection == 0)
    return -1;

  Points points = inputformat::formatFileContentsToCGALPoints(inputFile);

  // Polygon_2 polygon;

  // if (algorithm == "incremental")
  // {
  //   auto start = high_resolution_clock::now();
  //   polygon = incrementalAlgorithm(points, edge_selection, "1a");
  //   auto stop = high_resolution_clock::now();
  //   auto duration = duration_cast<microseconds>(stop - start);
  //   polygon_print(polygon, algorithm, edge_selection, 1, duration.count());
  // }

  // if (algorithm == "convex_hull")
  // {
  //   auto start = high_resolution_clock::now();
  //   polygon = convex_hull_Algorithm(points, edge_selection);
  //   auto stop = high_resolution_clock::now();
  //   auto duration = duration_cast<microseconds>(stop - start);
  //   polygon_print(polygon, algorithm, edge_selection, 1, duration.count());
  // }

  // if (algorithm == "onion")
  // {
  //   cout << "NOT IMPLEMENTED YET" << endl;
  // }

  if (algorithm == "local_search")
  {
    localSearch(points, MAX_PARAMETER, 0.1);
  }

  



  return 0;
}
