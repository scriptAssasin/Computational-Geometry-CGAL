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
  int L = 0;
  string MAX_PARAMETER = EMPTY_STRING;
  string MIN_PARAMETER = EMPTY_STRING;
  double threshold = 0.0;
  string annealing_option = EMPTY_STRING;
  double alpha = 0.0;
  double beta = 0.0;
  double ro = 0.0;
  bool elitism = false;
  string option = EMPTY_STRING;

  // command line arguments parsing
  
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
    if ((commandLineArguments[i] == "-L") && ((i + 1) < commandLineArguments.size()))
    {
      L = stoi(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-max") && ((i + 1) < commandLineArguments.size()))
    {
      MAX_PARAMETER = "max";
    }
    if ((commandLineArguments[i] == "-min") && ((i + 1) < commandLineArguments.size()))
    {
      MIN_PARAMETER = "min";
    }
    if ((commandLineArguments[i] == "-threshold") && ((i + 1) < commandLineArguments.size()))
    {
      threshold = stoi(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-annealing") && ((i + 1) < commandLineArguments.size()))
    {
      annealing_option.assign(commandLineArguments[i + 1]);
    }
   
    if ((commandLineArguments[i] == "-alpha") && ((i + 1) < commandLineArguments.size()))
    {
      annealing_option = stoi(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-beta") && ((i + 1) < commandLineArguments.size()))
    {
      beta = stoi(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-ro") && ((i + 1) < commandLineArguments.size()))
    {
      ro = stoi(commandLineArguments[i + 1]);
    }
    if ((commandLineArguments[i] == "-elitism") && ((i + 1) < commandLineArguments.size()))
    {
      elitism = stoi(commandLineArguments[i + 1]);
    }
  }

  // error checking
  if (inputFile == EMPTY_STRING || outputFile == EMPTY_STRING || algorithm == EMPTY_STRING
    || (algorithm == "local_search" && L != 0) || (algorithm != "local_search" && L == 0) 
    || (MAX_PARAMETER == EMPTY_STRING && MIN_PARAMETER == EMPTY_STRING) || 
    (threshold == 0.0 && algorithm == "local_search") || (annealing_option == EMPTY_STRING
    && algorithm == "simulated_annealing") || ((alpha == 0.0 || beta == 0.0 || ro == 0.0 ) 
    && algorithm == "ant_colony"))
    {
      cout << "Improper arguments" << endl;
      return -1;
    }
    

  int convex_hull_area;
  Points points = inputformat::formatFileContentsToCGALPoints(inputFile,convex_hull_area);

  Polygon_2 polygon;

  if (MAX_PARAMETER != EMPTY_STRING) {option = "max";}
  else {option = "min";}

  if (algorithm == "local_search")
  {
    auto start = high_resolution_clock::now();
    double initial_area;
    polygon = localSearch(points, option, threshold, initial_area);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    polygon_print(polygon, algorithm, option, duration.count(), initial_area, points, convex_hull_area, outputFile);
  }

  if (algorithm == "simulated_annealing")
  {
    auto start = high_resolution_clock::now();
    double initial_area;
    bool max_option; 
    max_option = (MAX_PARAMETER == "max") ? true : false;
    polygon = simulatedAnnealing(points, annealing_option, max_option, L, initial_area);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    polygon_print(polygon, algorithm, option, duration.count(), initial_area, points, convex_hull_area, outputFile);
  }
  
  if (algorithm == "ant_colony")
  {
    antColony(points);
  }

  return 0;
}
