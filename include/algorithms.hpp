#include <iostream>
#include <string>
#include <vector>
#include <CGAL/Polygon_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Segment_2 Segment_2;
typedef std::vector<Point_2> Points;
typedef CGAL::Polygon_2<K> Polygon_2;

using namespace std;

const string MAX_PARAMETER = "MAX";
const string MIN_PARAMETER = "MIN";

Polygon_2 incrementalAlgorithm(Points points, int edge_selection, string initialization);
Polygon_2 convex_hull_Algorithm(Points points, int edge_selection);
Polygon_2 onion(Points points, int edge_selection);

void localSearch(Points points, string parameter, double threshold);
void simulatedAnnealing(Points points, string parameter);
void antColony(Points points);