#include <iostream>
#include <climits>
#include <tuple>
#include <initializer_list>

#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"
#include <CGAL/Search_traits_2.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/Fuzzy_iso_box.h>

typedef CGAL::Vector_3<K> Vector_3;
typedef CGAL::Search_traits_2<K> Traits;
typedef CGAL::Kd_tree<Traits> Tree;
typedef CGAL::Fuzzy_iso_box<Traits> Fuzzy_iso_box;

const int L = 3;

int triangleOrientation(Point_2 &a, Point_2 &b, Point_2 &c)
{
    int orientation = (b.y() - a.y()) * (c.x() - b.x()) - (b.x() - a.x()) * (c.y() - b.y());
    if (orientation == 0)
        return 0;
    return orientation < 0 ? 1 : -1; // 1 for counter-clockwise, -1 for clockwise
}

template <class T>
int minimum_or_maximum_coordinates(std::initializer_list<T> list, bool max = false, bool x = true)
{
    int result = max ? INT_MIN : INT_MAX;
    // cout << "--------!" << result << "!---------" << endl;

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
                    result = point.x();
                }
            }
            else
            {
                if (point.y() < result)
                {
                    result = point.x();
                }
            }
        }
    }

    return result;
}

void getConvexHullPolygonFromPoints(const Points &vertices, Polygon_2 &convexHullPolygon)
{
    Points points;
    CGAL::convex_hull_2(vertices.begin(), vertices.end(), std::back_inserter(points));
    for (auto it = points.begin(); it != points.end(); it++)
        convexHullPolygon.push_back(*it);
}

void swapTwoPoints(Polygon_2 &polygon, int indexOfFirstPoint)
{

    // Let p, q, r, s be four consecutive points in polygon. We remove point q and place it before s
    // indexOfFirstPoint points to q

    Polygon_2::Vertex_iterator vertex = polygon.begin() + indexOfFirstPoint;
    Point_2 q = Point_2(*vertex);

    // remove q point
    polygon.erase(vertex);

    vertex = polygon.begin() + indexOfFirstPoint + 1;

    // add point q after r (or before s)
    polygon.insert(vertex, q);
}

bool checkIntersections(Polygon_2 &polygon, int randomPointIndex, Point_2 p, Point_2 q, Point_2 r, Point_2 s, Points pointsInBox)
{
    // new lines
    Segment_2 line1 = Segment_2(p, r);
    Segment_2 line2 = Segment_2(q, s);

    // checks if the line before p intersects with line q s
    Point_2 t;
    if (randomPointIndex - 2 >= 0)
        t = polygon[randomPointIndex - 2];
    else
        t = polygon[polygon.size() - 1]; // if p is the first point in polygon
    Segment_2 lineT = Segment_2(t, p);
    if (CGAL::do_intersect(line2, lineT))
        return true;

    // checks if the line after s intersects with line p r
    if (randomPointIndex + 3 < polygon.size())
        t = polygon[randomPointIndex + 3];
    else
        t = polygon[0]; // if s is the last point in polygon
    lineT = Segment_2(s, t);
    if (CGAL::do_intersect(line1, lineT))
        return true;

    // if new lines intersect each other try again
    if (CGAL::do_intersect(line1, line2))
        return true;
    bool validPointSwap;

    // check if lines from points in box intersect with new lines
    for (Polygon_2::Vertex_iterator vertex = polygon.begin(); vertex != polygon.end(); vertex++)
    {

        // validPointSwap = true;

        for (std::vector<Point_2>::iterator point = pointsInBox.begin(); point != pointsInBox.end(); point++)
        {
            if (*point == *vertex)
            {
                Point_2 a = *vertex; // point in box
                --vertex;
                Point_2 b = *vertex; // previous point
                ++vertex;
                ++vertex;
                Point_2 c = *vertex; // next point
                --vertex;

                if (a == q || b == q || c == q || a == r || b == r || c == r)
                    continue;

                // the two lines from point in box
                Segment_2 lineA = Segment_2(b, a);
                Segment_2 lineB = Segment_2(a, c);

                pointsInBox.erase(point);

                if (CGAL::do_intersect(line1, lineA) || CGAL::do_intersect(line1, lineB) || CGAL::do_intersect(line2, lineA) || CGAL::do_intersect(line2, lineB))
                    return true;

                break;
            }
        }

 
    }

    return false;
}

int localTransitionStep(Polygon_2 &polygon, double &changeOfPolygonArea)
{
    int randomPointIndex;
    // create kd-Tree
    Tree kdTree;
    for (Point_2 vertex : polygon.vertices())
        kdTree.insert(vertex);
    kdTree.build();

    bool isValidPointSwap;

    while (1)
    {
        // take a random point in polygon to swap
        randomPointIndex = 1 + (rand() % (polygon.size() - 3));
        // q and r are the points to be exchanged. p is previous to q and s is next to r
        Point_2 p = polygon[randomPointIndex - 1], q = polygon[randomPointIndex], r = polygon[randomPointIndex + 1], s = polygon[randomPointIndex + 2];

        int maxX = minimum_or_maximum_coordinates({p, q, r, s}, true);
        int minX = minimum_or_maximum_coordinates({p, q, r, s}, false);
        int maxY = minimum_or_maximum_coordinates({p, q, r, s}, true, false);
        int minY = minimum_or_maximum_coordinates({p, q, r, s}, false, false);

        Points pointsInBox;
        Fuzzy_iso_box searchBox(Point_2(minX, minY), Point_2(maxX, maxY));
        kdTree.search(back_inserter(pointsInBox), searchBox);

        bool hasIntersect = checkIntersections(polygon, randomPointIndex, p, q, r, s, pointsInBox);

        if (hasIntersect)
            continue;

        // swap points
        swapTwoPoints(polygon, randomPointIndex);

        double areaRemoved, areaAdded;

        //	calculate the area removed and area add because of swap
        //	if triangle p, q, r has the same orientation as the polygon
        // 	then the area of the triangle is inside the polygon
        //	else it is outside.
        if (polygon.orientation() == triangleOrientation(p, q, r))
        {
            areaRemoved = CGAL::area(p, q, r);
            areaAdded = CGAL::area(q, r, s);
        }
        else
        {
            areaRemoved = CGAL::area(q, r, s);
            areaAdded = CGAL::area(p, q, r);
        }

        changeOfPolygonArea = areaAdded - areaRemoved;

        // utils::printOutput2(polygon, pointsInBox, p, q, r, s);
        //  if(!polygon.is_simple()){
        //  	std::cout << "Not ok " << randomPointIndex << "\n";
        //  	exit(1);
        //  }
        //  else

        break;
    }

    return randomPointIndex;
}

void globalTransitionStep(Polygon_2 &polygon, double &changeOfPolygonArea, int indexOfPoint, int indexOfNewPosition)
{
}

void simulatedAnnealing(Points points, string parameter)
{
    const bool max = true;
    srand(time(0));
    cout << "HELLO WORLD SIMULATED ANNEALING!" << endl;

    Polygon_2 polygon = convex_hull_Algorithm(points, 3), convexHullPolygon;
    getConvexHullPolygonFromPoints(polygon.vertices(), convexHullPolygon);

    cout << "Area before: " << polygon.area() << endl;

    double energy = 0, temperature = 1, DE, R = rand() / (RAND_MAX);
    double convexHullArea = convexHullPolygon.area();
    double polygonArea = polygon.area();
    double changeOfPolygonArea, energyAfterStep;

    cout << "Convex Hull Area: " << convexHullArea << endl;

    // calculate energy
    energy = points.size() * polygonArea / convexHullArea;

    for (int i = 0; i < L; i++)
    {
        cout << "iteration: " << L << endl;

        int indexOfFirstPoint;  // the index of the point, that randomly choosen to be swapped with its next (for both steps)
        int indexOfNewPosition; // point to be moved, will be inserted before point with this index (only for global step)

        if (string("local").compare(parameter) == 0)
        {
            indexOfFirstPoint = localTransitionStep(polygon, changeOfPolygonArea);
        }
        // else
        // {
        //     globalTransitionStep(polygon, changeOfPolygonArea, indexOfFirstPoint, indexOfNewPosition);
        // }

        // calculate new area
        polygonArea += changeOfPolygonArea;

        // calculate new energy
        energyAfterStep = points.size() * (polygonArea / convexHullArea);
        DE = energyAfterStep - energy;

        // check if step is acceptable
        if ((DE > 0 && max) || (DE < 0 && !max) || exp(-DE / temperature > R))
            energy = energyAfterStep;
        else
        {
            // if step is not acceptable, undo the step
            if (string("local").compare(parameter) == 0)
                swapTwoPoints(polygon, indexOfFirstPoint);
            // else
            //     changePositionOfPoint(polygon, indexOfNewPosition, indexOfFirstPoint);
            polygonArea -= changeOfPolygonArea;
        }

        // reduce temperature
        temperature = temperature - 1 / L;
    }

    std::cout << "Area after: " << polygon.area() << std::endl;

    std::cout << "success\n";
}
