#include <iostream>
#include "../include/algorithms.hpp"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/enum.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/squared_distance_2.h>
#include <iterator>
#include <climits>
#include <vector>
#include <tuple>
#include <boost/optional/optional_io.hpp>
#include "../include/polygonizationHelper.hpp"

using namespace std;

void antColony(Points points)
{
    Polygon_2 polygon;
    polygon = incrementalAlgorithm(points, 2, "1a");
    cout << polygon << endl;

    const int alpha = 1;
    const int beta = 1;
    const int ro = 1;
    const int evaporation = 1; 
    const int M = 30;
    const int N = 20;

    cout << "HELLO WORLD FROM ANT COLONY" << endl;
}