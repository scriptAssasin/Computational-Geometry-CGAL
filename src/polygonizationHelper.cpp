#include "../include/polygonizationHelper.hpp"
#include <fstream>

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
        if (p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)e1[0] &&
            p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)e1[1] &&
            p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)pol_edge[0] &&
            p1 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)pol_edge[1])
        {
            return false;
        }
    }
    if (p2)
    {
        if (p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)e2[0] &&
            p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)e2[1] &&
            p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)pol_edge[0] &&
            p2 != (boost::optional<boost::variant<CGAL::Point_2<CGAL::Epick>, CGAL::Segment_2<CGAL::Epick>>>)pol_edge[1])
        {
            return false;
        }
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
        if (intersection_clause(e, e1, e2, pol_edge))
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
    for (int i = 0; i < points.size() - 1; i++)
    {

        Segment_2 s(points[i], points[i + 1]);
        edges.push_back(s);
    }
    return edges;
}

bool is_red_edge(Segment_2 e, Point_2 p, Polygon_2 pol)
{
    return is_visible(pol, p, e);
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
            if (red_edges[i][0] == pol_edge[0] && (!(count(purple_points.begin(), purple_points.end(), red_edges[i][0]))))
            {
                purple_points.push_back(red_edges[i][0]);
            }
            else if (red_edges[i][1] == pol_edge[0] && (!(count(purple_points.begin(), purple_points.end(), red_edges[i][1]))))
            {
                purple_points.push_back(red_edges[i][1]);
            }
            else if (red_edges[i][0] == pol_edge[1] && (!(count(purple_points.begin(), purple_points.end(), red_edges[i][0]))))
            {
                purple_points.push_back(red_edges[i][0]);
            }
            else if (red_edges[i][1] == pol_edge[1] && (!(count(purple_points.begin(), purple_points.end(), red_edges[i][1]))))
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
        return Segment_2(Point_2(4, 2), Point_2(2, 4));
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
                distance = CGAL::squared_distance(inside_points[i], *iter);
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
            if (!polygon.is_simple())
            {
                continue;
            }
            polygon = old_polygon;
        }
        if (!flag)
            return -1;

        polygon = old_polygon;
        update_polygon(selected_edge, selected_point, polygon);
        // remove current point that was added to polygon
        for (auto it = inside_points.begin(); it != inside_points.end(); ++it)
        {
            // cout << *it << endl;
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
                distance = CGAL::squared_distance(inside_points[i], *iter);
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
            if (!polygon.is_simple())
            {
                continue;
            }
            polygon = old_polygon;
        }
        if (!flag)
            return -1;
        polygon = old_polygon;
        update_polygon(selected_edge, selected_point, polygon);
        for (auto it = inside_points.begin(); it != inside_points.end(); ++it)
        {
            // cout << *it << endl;
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
                distance = CGAL::squared_distance(inside_points[i], *iter);
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
        // visible_edges.erase(unique(visible_edges.begin(), visible_edges.end()), visible_edges.end());
        // selected_points.erase(unique(selected_points.begin(), selected_points.end()), selected_points.end());
        if (visible_edges.empty())
            return -1;
        else
        {
            int randomIndex = rand() % selected_points.size();
            update_polygon(visible_edges[randomIndex], selected_points[randomIndex], polygon);
            if (!polygon.is_simple())
            {
                return -1;
            }
            for (auto it = inside_points.begin(); it != inside_points.end(); ++it)
            {
                // cout << *it << endl;
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
            if (polygon.bounded_side(Point_2(x, y)) == CGAL::ON_BOUNDED_SIDE) // internal point
                insidePoints.push_back(Point_2(x, y));
            else if (polygon.bounded_side(Point_2(x, y)) == CGAL::ON_BOUNDARY)
                outsidePoints.push_back(Point_2(x, y));
            y++;
        }
        x++;
    }

    // cout << (double)insidePoints.size() + (((double)outsidePoints.size()) / ((double)2)) - 1.0 << endl;
    return ((double)insidePoints.size() + (((double)outsidePoints.size()) / ((double)2)) - 1.0);
}

void polygon_print(Polygon_2 polygon, string algorithm, string option, int time, double initial_area,
                                         Points points, int convex_hull_area, string filename)
{

    ofstream ouput(filename);
    
    if (polygon.is_empty())
    {
        ouput << "ERROR: No available polygon for given data!" << endl;
        return;
    }

    ouput << "Optimal Area Polygonization" << endl;
    // print edges and vertices of polygon
    for (vertexit iter = polygon.vertices_begin(); iter != polygon.vertices_end(); ++iter)
    {
        ouput << *iter << endl;
    }

    for (edgeit iter = polygon.edges_begin(); iter != polygon.edges_end(); ++iter)
    {
        ouput << *iter << endl;
    }
    
     bool IsSimple = polygon.is_simple();
    ouput << "isSimplePolygon: " << IsSimple << endl;

    ouput << "Algorithm: " << algorithm << "_" << option << endl;

    double area = polygon.area();

    if (algorithm != "ant_colony")
    {
        ouput << "area_initial: " << initial_area << endl;
    }
    
    ouput << "area: " << area << endl;

    area = (int)area;
    initial_area = (int)initial_area;
    
    if (algorithm != "ant_colony")
    {
        ouput << "ratio_initial: " << initial_area/convex_hull_area << endl;
    }
    
    ouput << "ratio: " << area/convex_hull_area << endl;
    
    ouput << "construction time: " << time << endl;
}

Polygon_2 get_convex_hull_polygon(Points vertices)
{
    Points points;
    Polygon_2 convexHull;
    CGAL::convex_hull_2(vertices.begin(), vertices.end(), back_inserter(points));
    for (auto it = points.begin(); it != points.end(); it++)
        convexHull.push_back(*it);

    return convexHull;
}

Polygon_2 swapPoints(Polygon_2 polygon, int startingPoint)
{
    Polygon_2 temp_polygon(polygon);
    vertexit vert = temp_polygon.begin() + startingPoint;
    Point_2 q(*vert);

    temp_polygon.erase(vert);

    vert = temp_polygon.begin() + startingPoint;
    vert += 1;

    temp_polygon.insert(vert, q);

    return temp_polygon;
}

Polygon_2 pointPositionChange(Polygon_2 polygon, int &pointIndex, int &newPos)
{
    Polygon_2 temp_polygon(polygon);
    Point_2 q = Point_2(*(temp_polygon.begin() + pointIndex));

    // remove point q from polygon
    temp_polygon.erase(temp_polygon.begin() + pointIndex);

    // insert q between s and t
    temp_polygon.insert(temp_polygon.begin() + newPos, q);

    return temp_polygon;
}

int triangleOrientation(Point_2 a, Point_2 b, Point_2 c)
{
    int pointsBandACoordinateXdifference = b.x() - a.x();
    int pointsBandACoordinateYdifference = b.y() - a.y();
    int pointsBandCCoordinateXdifference = c.x() - b.x();
    int pointsBandCCoordinateYdifference = c.y() - b.y();

    int orientation = pointsBandACoordinateYdifference * pointsBandCCoordinateXdifference - pointsBandACoordinateXdifference * pointsBandCCoordinateYdifference;
    if (orientation == 0)
        return 0;
    // 1 for counter-clockwise, -1 for clockwise
    return orientation < 0 ? 1 : -1;
}

// helper function that gets list of Points and return the minimum or maximum based on x or y depends on its arguments
template <class T>
int minimum_or_maximum_coordinates(std::initializer_list<T> list, bool max = false, bool x = true)
{
    int result = max ? INT_MIN : INT_MAX;

    for (auto point : list)
    {
        if (x)
        {

            if (max)
            {
                if (point.x() > result)
                {
                    result = point.x();
                }
            }
            else
            {
                if (point.x() < result)
                {
                    result = point.x();
                }
            }
        }
        else
        {
            if (max)
            {
                if (point.y() > result)
                {
                    result = point.y();
                }
            }
            else
            {
                if (point.y() < result)
                {
                    result = point.y();
                }
            }
        }
    }

    return result;
}

Point_2 get_minimum_point(Point_2 a, Point_2 b, Point_2 c, Point_2 d)
{
    // find minimum x,y using our helper function
    int minimumXCoordinate = minimum_or_maximum_coordinates({a, b, c, d}, false);
    int minimumYCoordinate = minimum_or_maximum_coordinates({a, b, c, d}, false, false);

    return Point_2(minimumXCoordinate, minimumYCoordinate);
}

Point_2 get_maximum_point(Point_2 a, Point_2 b, Point_2 c, Point_2 d)
{
    // find maximum x,y using our helper function
    int maximumXCoordinate = minimum_or_maximum_coordinates({a, b, c, d}, true);
    int maximumYCoordinate = minimum_or_maximum_coordinates({a, b, c, d}, true, false);

    return Point_2(maximumXCoordinate, maximumYCoordinate);
}

Points get_points_in_box(Tree &kdTree, Point_2 a, Point_2 b, Point_2 c, Point_2 d)
{
    // find the points of polygon in the box
    Points pointsInBox;
    Fuzzy_iso_box searchBox(get_minimum_point(a, b, c, d), get_maximum_point(a, b, c, d));
    kdTree.search(back_inserter(pointsInBox), searchBox);

    return pointsInBox;
}

bool checkIntersections(Polygon_2 &polygon, int randomPoint, Point_2 p, Point_2 q, Point_2 r, Point_2 s, Points pointsInBox)
{
    Segment_2 linePR = Segment_2(p, r), lineQS = Segment_2(q, s), lineTemp;
    Point_2 temp;
    Point_2 polygon_last_point = polygon[polygon.size() - 1], polygon_first_point = polygon[0];
    // checks if the line which is before p intersects with line qs
    temp = randomPoint - 2 >= 0 ? polygon[randomPoint - 2] : polygon_last_point;
    lineTemp = Segment_2(temp, p);
    if (CGAL::do_intersect(lineQS, lineTemp))
        return true;
    // checks if the line which is after s intersects with line pr
    temp = randomPoint + 3 < polygon.size() ? polygon[randomPoint + 3] : polygon_first_point;
    lineTemp = Segment_2(s, temp);
    if (CGAL::do_intersect(linePR, lineTemp))
        return true;
    // if new lines intersect each other try again
    if (CGAL::do_intersect(linePR, lineQS))
        return true;
    // check if lines from points in box intersect with new lines
    for (vertexit vertex_iterator = polygon.begin(); vertex_iterator != polygon.end(); vertex_iterator++)
        for (pveciterator point_iterator = pointsInBox.begin(); point_iterator != pointsInBox.end(); point_iterator++)
        {
            if (*point_iterator == *vertex_iterator)
            {
                // Call Point_2 copy constructor
                Point_2 point_in_box(*vertex_iterator), previous_point(*(vertex_iterator - 1)), next_point(*(vertex_iterator + 1));

                if (point_in_box == q || point_in_box == r)
                    continue;
                if (previous_point == q || next_point == q || previous_point == r || next_point == r)
                    continue;
                if (next_point == q || next_point == r)
                    continue;
                // the two lines from point in box
                Segment_2 linePreviousPointToCurrentPoint = Segment_2(previous_point, point_in_box), lineCurrentPointToNextPoint = Segment_2(point_in_box, next_point);
                if (CGAL::do_intersect(linePR, linePreviousPointToCurrentPoint) || CGAL::do_intersect(linePR, lineCurrentPointToNextPoint) || CGAL::do_intersect(lineQS, linePreviousPointToCurrentPoint) || CGAL::do_intersect(lineQS, lineCurrentPointToNextPoint))
                    return true;
                // remove point from pointsInBox
                pointsInBox.erase(point_iterator);
                break;
            }
        }
    return false;
}

// function that performs move step of V,e in local search
void move(Polygon_2 &starting_polygon, Points &polygon_line, Segment_2 edge)
{

    vertexit starting_point_iterator, ending_point_iterator;
    vertexit edge_index;

    // calculating iterators
    for (vertexit vertit = starting_polygon.vertices_begin(); vertit != starting_polygon.vertices_end(); ++vertit)
    {
        if (*vertit == polygon_line[0])
        {
            starting_point_iterator = vertit;
        }
        if (*vertit == polygon_line[polygon_line.size() - 1])
        {
            ending_point_iterator = next(vertit);
        }
    }

    if ((starting_point_iterator == starting_polygon.vertices_end() - 1))
    {
    }
    else if (starting_point_iterator == starting_polygon.vertices_end() - 2)
    {
    }
    else
    {
        // swapping is performed by removing V and inserting it to e
        starting_polygon.erase(starting_point_iterator, ending_point_iterator);

        // recalculating iterators after swap
        for (vertexit vertit = starting_polygon.vertices_begin(); vertit != starting_polygon.vertices_end(); ++vertit)
        {
            if (*vertit == edge[0])
            {
                edge_index = vertit;
            }
        }

        starting_polygon.insert(edge_index, polygon_line.begin(), polygon_line.end());
    }

    return;
}

// function that creates k-sequences of vertices (polygon line)
vector<Points> calculate_polygon_lines(Polygon_2 &polygon)
{

    vector<Points> polygon_lines;

    for (vertexit vertit = polygon.vertices_begin(); vertit != polygon.vertices_end(); ++vertit)
    {
        Points polygon_line;

        // edge cases
        if (vertit == prev(prev(polygon.vertices_end())))
        {
            polygon_line.push_back(*vertit);
            polygon_line.push_back(*(vertit + 1));
            polygon_line.push_back(*(polygon.vertices_begin()));
        }
        else if (vertit == prev(polygon.vertices_end()))
        {
            polygon_line.push_back(*vertit);
            polygon_line.push_back(*(polygon.vertices_begin()));
            polygon_line.push_back(*next(polygon.vertices_begin()));
        }
        else
        {
            polygon_line.push_back(*vertit);
            polygon_line.push_back(*(vertit + 1));
            polygon_line.push_back(*(vertit + 2));
        }

        polygon_lines.push_back(polygon_line);
    }

    return polygon_lines;
}