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

Polygon_2 convex_hull_Algorithm(Points points, int edge_selection)
{
        
    Points convex_hull;
    Polygon_2 polygon;
    Points internal_points;
    CGAL::convex_hull_2(points.begin(), points.end(), back_inserter(convex_hull));
    
    //polygon = convex_hull
    
    for (int i=0; i < convex_hull.size(); ++i)
    {
        polygon.push_back(convex_hull[i]);
    }
    
    for (int i=0; i < points.size(); ++i)
    {
        if (polygon.bounded_side(points[i]) == CGAL::ON_BOUNDED_SIDE)  //internal point
            internal_points.push_back(points[i]);
    }
    
    
    while (internal_points.size() > 0)
    {
        //update polygon and insert visible edge from internal point
        
        if (insert_edge_with_criteria(polygon,internal_points,edge_selection) != 1)
        {
            cout << "CANNOT CREATE POLYGON: NO AVAILABLE EDGE" << endl;
            Polygon_2 err_pol;
            return err_pol;
        }
        
    }

    return polygon;
}