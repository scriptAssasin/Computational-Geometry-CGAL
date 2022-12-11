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
#include "../include/polygonizationHelper.hpp"

using namespace std;

Polygon_2 incrementalAlgorithm(Points points, int edge_selection, string initialization)
{
    
    Points current_convex_hull;

    // sort points based on x
    sort(points.begin(), points.end(), [](Point_2 a, Point_2 b)
         { return a.x() < b.x(); });

    Polygon_2 polygon;

    //initial polygon
    polygon.push_back(points[0]); 
    polygon.push_back(points[1]);
    polygon.push_back(points[2]);


    Points points_visited_so_far;
    points_visited_so_far.push_back(points[0]);
    points_visited_so_far.push_back(points[1]);
    points_visited_so_far.push_back(points[2]);


    // current_convex_hull
    CGAL::convex_hull_2(points_visited_so_far.begin(), points_visited_so_far.end(), back_inserter(current_convex_hull));
    
    for (pveciterator iter = next(next(next(points.begin()))); iter != points.end(); ++iter)
    {
        points_visited_so_far.push_back(*iter);

        // update convex_hull
        CGAL::convex_hull_2(points_visited_so_far.begin(), points_visited_so_far.end(), back_inserter(current_convex_hull));

        // data structure for red_edges
        vector<Segment_2> red_edges;
        // loop edges of convex_hull(pol_edge)
        vector<Segment_2> convex_hull_edges = make_edges(current_convex_hull);
        for (int i = 0; i < convex_hull_edges.size(); i++)
        {
            Segment_2 pol_edge = convex_hull_edges[i];
            if (is_red_edge(pol_edge, *iter, polygon))
            {
                red_edges.push_back(pol_edge);
            }
        }
        // loop red_edges (re_i)
        vector<Segment_2> visible_edges;
        
        visible_edges = return_visible_edges(red_edges, polygon, *iter);

        // select visible edge
        Segment_2 e = select_edge_with_criteria(polygon, visible_edges, *iter, edge_selection);

        // insert new vertex in polygon
        update_polygon(e, *iter, polygon);
        
    }
    return polygon;
}