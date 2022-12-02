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
#include <cstdlib>
#include <algorithm>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Triangle_2<K> Triangle_2; //?
typedef K::Segment_2 Segment_2;
typedef Polygon_2::Edge_const_iterator edgeit;
typedef Polygon_2::Vertex_iterator vertexit;
typedef std::vector<Point_2> Points;

using namespace std;


// function that determines if a two edges intersect (with edge cases)
bool intersection_clause(Segment_2 e, Segment_2 e1, Segment_2 e2, Segment_2 pol_edge) 
{
    
    // with itself
    if (pol_edge == e)
        return true;
    
    auto p1 = CGAL::intersection(pol_edge, e1);
    auto p2 = CGAL::intersection(pol_edge, e2);
    
    // with the current edges
    if (p1)
    {
        if (p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) e1[0] &&
         p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) e1[1] && 
         p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) pol_edge[0] && 
         p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) pol_edge[1])
            {return false;}
    }
    if (p2)
    {
        if (p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) e2[0] &&
         p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) e2[1] && 
         p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) pol_edge[0] && 
         p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>) pol_edge[1])
            {return false;}
    }

    
    return true;
}

bool is_visible(Polygon_2 polygon, Point_2 p, Segment_2 e)
{
    Segment_2 e1(e[0], p);
    Segment_2 e2(e[1], p);
    
    // loop edges of polygon (pol_edge)
    for (const Segment_2 &pol_edge : polygon.edges())
    {
        if ( intersection_clause(e,e1,e2,pol_edge) )
            continue;
        else
            return false;
    }
    return true;
}

void update_polygon(Segment_2 e, Point_2 p, Polygon_2 &pol)
{

    for (vertexit iter = pol.vertices_begin(); iter != pol.vertices_end(); ++iter)
    {
        if (*iter == e[0])
        {

            if (iter == pol.vertices_end())
            {
                pol.insert(pol.vertices_begin(), p);
                break;
            }
            pol.insert(iter + 1, p);
            break;
        }
    }
    return;
}

// create a list if aedges form a list of vertices(cycle)
vector<Segment_2> make_edges(Points points)
{
    vector<Segment_2> edges;
    for (int i = 0; i < points.size(); i++)
    {
        if (i == points.size() - 1)
        {
            Segment_2 s(points[i], points[0]);
            edges.push_back(s);
            break;
        }
        Segment_2 s(points[i], points[i + 1]);
        edges.push_back(s);
    }
    return edges;
}

// not cycle
vector<Segment_2> make_edges_simple(Points points)
{
    vector<Segment_2> edges;
    for (int i = 0; i < points.size()-1; i++)
    {
        
        Segment_2 s(points[i], points[i + 1]);
        edges.push_back(s);
    }
    return edges;
}


bool is_red_edge(Segment_2 e, Point_2 p, Polygon_2 pol)
{
    return is_visible(pol,p,e);
}

// select the visible edges based on the red edges
vector<Segment_2> return_visible_edges(vector<Segment_2> red_edges, Polygon_2 polygon, Point_2 p)
{
    vector<Segment_2> green_edges;
    Points green_points;
    vector<Segment_2> visible_edges;
    Points purple_points;                             
    
    for (const Segment_2 &pol_edge : polygon.edges()) 
    {
        for (int i = 0; i < red_edges.size(); ++i)
        {
            // find purple points that red and blue edges meet
            if (red_edges[i][0] == pol_edge[0] && (!(count(purple_points.begin(),purple_points.end(),red_edges[i][0]))))
            {
                purple_points.push_back(red_edges[i][0]);
            }
            else if (red_edges[i][1] == pol_edge[0] && (!(count(purple_points.begin(),purple_points.end(),red_edges[i][1]))))
            {
                purple_points.push_back(red_edges[i][1]);
            }
            else if (red_edges[i][0] == pol_edge[1] && (!(count(purple_points.begin(),purple_points.end(),red_edges[i][0]))))
            {
                purple_points.push_back(red_edges[i][0]);
            }
            else if (red_edges[i][1] == pol_edge[1] && (!(count(purple_points.begin(),purple_points.end(),red_edges[i][1]))))
            {
                purple_points.push_back(red_edges[i][1]);
            }
        }
    }

    sort(purple_points.begin(), purple_points.end(), [](Point_2 a, Point_2 b)
         { return a.x() < b.x(); });

    // flag to iterate between purple points
    bool flag = false;
    for (vertexit iter = polygon.vertices_begin(); iter != polygon.vertices_end(); ++iter)
    {
        
        if (*iter == purple_points[0] || *iter == purple_points[1]) 
        {
            flag = 1;
        }
        if (flag)
        {
            green_points.push_back(*iter);
        }
        if (*iter == purple_points[1] || *iter == purple_points[0])
        {
            flag = 0;
        }
    }
    
    green_edges = make_edges_simple(green_points);
    for (int i = 0; i < green_edges.size(); ++i)
    {
        if (is_visible(polygon, p, green_edges[i]))
        {
            visible_edges.push_back(green_edges[i]);
            
        }     
    }
    return visible_edges;
}

Segment_2 select_edge_with_criteria(Polygon_2 polygon, vector<Segment_2> visible_edges, Point_2 p, int criteria)
{
    
    if (visible_edges.empty())
    {
        return Segment_2(Point_2(4,2),Point_2(2,4));
    }
    
    Segment_2 selected_edge;
    Polygon_2 old_polygon = polygon;
    switch (criteria)
    {
        // max area
        case 1:
        {
            int max_area = 0;
            for (int i = 0; i < visible_edges.size(); i++)
            {
                Segment_2 current_edge = visible_edges[i];
                
                update_polygon(current_edge, p, polygon);
                int current_area = polygon.area();
                if (current_area > max_area)
                {
                    max_area = current_area;
                    selected_edge = current_edge;
                }
                polygon = old_polygon; //? does this work or does it need constructor etc
            }
            polygon = old_polygon;
            break;
        }
        // min area
        case 2:
        {
            int min_area = INT_MAX;
            old_polygon = polygon;
            for (int i = 0; i < visible_edges.size(); i++)
            {
                Segment_2 current_edge = visible_edges[i];
                update_polygon(current_edge, p, polygon);
                int current_area = polygon.area();
                if (current_area < min_area)
                {
                    min_area = current_area;
                    selected_edge = current_edge;
                }
                polygon = old_polygon;
            }
            polygon = old_polygon;
            break;
        }
        // random selection
        case 3:
        {
            int randomIndex = rand() % visible_edges.size();
            selected_edge = visible_edges[randomIndex];
            break;
        }
    }
    return selected_edge;
}


Points sortPoints(Points points, string initialization)
{
    if (initialization == "1a")
    {
        sort(points.begin(), points.end(), [](Point_2 a, Point_2 b)
             { return a.x() > b.x(); });
    }
    else if (initialization == "1b")
    {
        sort(points.begin(), points.end(), [](Point_2 a, Point_2 b)
             { return a.x() < b.x(); });
    }
    else if (initialization == "2a")
    {
        sort(points.begin(), points.end(), [](Point_2 a, Point_2 b)
             { return a.y() > b.y(); });
    }
    else if (initialization == "2b")
    {
        sort(points.begin(), points.end(), [](Point_2 a, Point_2 b)
             { return a.y() < b.y(); });
    }

    return points;
}

int insert_edge_with_criteria(Polygon_2 &polygon, Points &inside_points, int criteria)
{
    
    Segment_2 selected_edge;
    Polygon_2 old_polygon = polygon;
    Point_2 selected_point;
    bool flag = false;

    switch (criteria)
    {
        // max area
        case 1:
        {
            double max_area = 0.0;
            // loop all edges and find closest point for each one
            for (edgeit iter = polygon.edges_begin(); iter != polygon.edges_end(); ++iter)
            {
                double min_dist = DBL_MAX;
                double distance;
                Point_2 p;
                for (int i = 0; i < inside_points.size(); ++i)
                {
                    distance = CGAL::squared_distance(inside_points[i],*iter);
                    if (distance < min_dist)
                    {
                        min_dist = distance;
                        p = inside_points[i];
                    }
                }
                // if edge is visible it is a potential selection
                if (is_visible(polygon, p, *iter))
                {
                    flag = true;
                    update_polygon(*iter, p, polygon);
                    double current_area = polygon.area();
                    if (current_area > max_area)
                    {
                        max_area = current_area;
                        selected_edge = *iter;
                        selected_point = p;
                    }
                }

                polygon = old_polygon; 
            }
            if (!flag)
                return -1;
            polygon = old_polygon;
            update_polygon(selected_edge, selected_point, polygon);
            // remove current point that was added to polygon
            for (auto it=inside_points.begin(); it!=inside_points.end(); ++it)
            {
                    cout << *it << endl;
                    if (*it == selected_point)
                    {
                        inside_points.erase(it);
                        break;
                    }
            }
            break;
        }
        // min area
        case 2:
        {
            double min_area = DBL_MAX;
            for (edgeit iter = polygon.edges_begin(); iter != polygon.edges_end(); ++iter)
            {
                double min_dist = DBL_MAX;
                double distance;
                Point_2 p;
                for (int i = 0; i < inside_points.size(); ++i)
                {
                    distance = CGAL::squared_distance(inside_points[i],*iter);
                    if (distance < min_dist)
                    {
                        min_dist = distance;
                        p = inside_points[i];
                    }
                }
                if (is_visible(polygon, p, *iter))
                {
                    flag = true;
                    update_polygon(*iter, p, polygon);
                    double current_area = polygon.area();
                    if (current_area < min_area)
                    {
                        min_area = current_area;
                        selected_edge = *iter;
                        selected_point = p;
                    }
                }

                polygon = old_polygon;
            }
            if (!flag)
                return -1;
            polygon = old_polygon;
            update_polygon(selected_edge, selected_point, polygon);
            for (auto it=inside_points.begin(); it!=inside_points.end(); ++it)
            {
                    cout << *it << endl;
                    if (*it == selected_point)
                    {
                        inside_points.erase(it);
                        break;
                    }
            }
            break;
        }
        // random selection
        case 3:
        {
            vector<Segment_2> visible_edges;
            Points selected_points;
            int counter = 0;
            for (edgeit iter = polygon.edges_begin(); iter != polygon.edges_end(); ++iter)
            {
                double min_dist = DBL_MAX;
                double distance;
                Point_2 p;
                for (int i = 0; i < inside_points.size(); ++i)
                {
                    distance = CGAL::squared_distance(inside_points[i],*iter);
                    if (distance < min_dist)
                    {
                        min_dist = distance;
                        p = inside_points[i];
                    }
                }
                if (is_visible(polygon, p, *iter))
                {
                    visible_edges.push_back(*iter);
                    selected_points.push_back(p);
                }
                
            }
            visible_edges.erase( unique(visible_edges.begin(),visible_edges.end()), visible_edges.end());
            selected_points.erase( unique(selected_points.begin(),selected_points.end()), selected_points.end());
            if (visible_edges.empty())
                return -1;
            else 
            {
                int randomIndex = rand() % selected_points.size();
                update_polygon(visible_edges[randomIndex],selected_points[randomIndex],polygon);
                for (auto it=inside_points.begin(); it!=inside_points.end(); ++it)
                {
                    cout << *it << endl;
                    if (*it == selected_points[randomIndex])
                    {
                        inside_points.erase(it);
                        break;
                    }
                }
            }
            break;
        }
    }
    return 1;
}


double pick_algorithm(Polygon_2 polygon)
{
    double area;
    Points insidePoints;
    Points outsidePoints;

    // define grid
    int min_x = (*polygon.left_vertex())[0];
    int min_y = (*polygon.bottom_vertex())[1];
    int max_x = (*polygon.right_vertex())[0];
    int max_y = (*polygon.top_vertex())[1];

    
    for (int x = min_x; x <= max_x; x++)
    {
        for (int y = min_y; y <= max_y; y++)
        {
            if (polygon.bounded_side(Point_2(x,y)) == CGAL::ON_BOUNDED_SIDE)  //internal point
                insidePoints.push_back(Point_2(x,y));
            else if (polygon.bounded_side(Point_2(x,y)) == CGAL::ON_BOUNDARY)
                outsidePoints.push_back(Point_2(x,y));
            y++;
        }
        x++;
    }
    
    cout << (double)insidePoints.size() + (((double)outsidePoints.size()) / ((double)2)) - 1.0 << endl;
    return ((double)insidePoints.size() + (((double)outsidePoints.size()) / ((double)2)) - 1.0);
}

void polygon_print(Polygon_2 polygon, string algorithm, int edge_selection, int initialization, int time)
{
    
    if (polygon.is_empty())
    {
        cout << "ERROR: No available polygon for given data!" << endl;
        return;
    }
        
    cout << "Polygonization" << endl;
    // print edges and vertices of polygon
    for (vertexit iter=polygon.vertices_begin(); iter!=polygon.vertices_end(); ++iter)
    {
        cout << *iter << endl;
    }

    for (edgeit iter=polygon.edges_begin(); iter!=polygon.edges_end(); ++iter)
    {
        cout << *iter << endl;
    }

    cout << "Algorithm: " << algorithm << "_edge_election: " << edge_selection << endl;
    if (initialization)
        cout << "initialization: " << initialization << endl;
    
    
    double area = polygon.area();
    
    
    bool IsSimple    = polygon.is_simple();
    bool IsConvex    = polygon.is_convex();
    bool IsClockwise = (polygon.orientation() == CGAL::CLOCKWISE);
    double Area      = abs(polygon.area()); 

    cout << "area: " << Area << endl;
    cout << "isSimple: " << IsSimple << endl;

    int pick_area = pick_algorithm(polygon);
    if (pick_area) 
    {
        cout << "pick_calculated_area: " << pick_area << endl;
        cout << "ratio: " << Area / pick_area << endl;
    }
    else
        cout << "Pick area calculated zero!" << endl;
    
    cout << "construction time: " << time << endl;
        
}