#include <iostream>
#include <string>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef vector<Point_2> Points;

using namespace std;

Polygon_2 incrementalAlgorithm(Points points, int edge_selection, string initialization);
Polygon_2 convex_hull_Algorithm(Points points, int edge_selection);
Polygon_2 onion(Points points, int edge_selection);