#pragma once
#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/enum.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/squared_distance_2.h>
#include <iterator>
#include <climits>
#include <vector>
#include <boost/optional/optional_io.hpp>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Triangle_2<K> Triangle_2; //?
typedef K::Segment_2 Segment_2;
typedef std::vector<Point_2>::iterator pveciterator;
typedef std::vector<Segment_2>::iterator sveciterator;
typedef std::vector<Point_2> Points;
typedef Polygon_2::Edge_const_iterator edgeit;
typedef Polygon_2::Vertex_iterator vertexit;

using namespace std;

bool is_visible(Polygon_2, Point_2, Segment_2);

void update_polygon(Segment_2, Point_2, Polygon_2 &);

vector<Segment_2> make_edges(vector<Point_2>);

bool is_red_edge(Segment_2, Point_2, Polygon_2);

vector<Segment_2> return_visible_edges(vector<Segment_2>, Polygon_2, Point_2);

Segment_2 select_edge_with_criteria(Polygon_2, vector<Segment_2>, Point_2, int);

vector<Point_2> sortPoints(vector<Point_2> points, string initialization);

int insert_edge_with_criteria(Polygon_2&, Points&, int);

void polygon_print(Polygon_2 polygon, string algorithm, int edge_selection, int initialization, int time);

Polygon_2 get_convex_hull_polygon(Points vertices);

void swapTwoPoints(Polygon_2 &polygon, int indexOfFirstPoint);