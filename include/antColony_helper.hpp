#include <iostream>
#include <tuple>
#include <map>
#include <cmath>
#include <ctime>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/enum.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/squared_distance_2.h>
#include <iterator>
#include <climits>
#include <vector>
#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"

void initialize(Points points, vector<tuple<Segment_2,int>> &trails);

Points feasablePoints(Polygon_2 polygon, Points points);

vector<Segment_2> visibleEdges(Polygon_2 polygon, Point_2 p);

Segment_2 selectEdge(Polygon_2 polygon, vector<Segment_2> visible_edges, Point_2 p, vector<tuple<Segment_2,int>> trails, int k, string option, double a, double b);

Polygon_2 build_solution(Points initial_points, vector<tuple<Segment_2,int>> trails, int k, string option, double a, double b);

void evaluate_solution(int k, Polygon_2 polygon, Polygon_2 &best_polygon_so_far, string option);

void SaveBestSolutionSoFar(int cycle, Polygon_2 polygon, Polygon_2 &best_polygon_so_far, string option);

void update_trails(int cycle, bool elitism, string option, vector<tuple<Segment_2,int>> &trails, double ro, int k, Polygon_2 polygon, Polygon_2 best_solution);
