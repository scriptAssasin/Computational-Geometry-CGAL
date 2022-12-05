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

int maxCoordinateX(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s)
{
    int max = p.x();
    if (q.x() > max)
        max = q.x();
    if (r.x() > max)
        max = r.x();
    if (s.x() > max)
        max = s.x();
    return max;
}

int maxCoordinateY(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s)
{
    int max = p.y();
    if (q.y() > max)
        max = q.y();
    if (r.y() > max)
        max = r.y();
    if (s.y() > max)
        max = s.y();
    return max;
}

int minCoordinateX(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s)
{
    int min = p.x();
    if (q.x() < min)
        min = q.x();
    if (r.x() < min)
        min = r.x();
    if (s.x() < min)
        min = s.x();
    return min;
}

int minCoordinateY(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s)
{
    int min = p.y();
    if (q.y() < min)
        min = q.y();
    if (r.y() < min)
        min = r.y();
    if (s.y() < min)
        min = s.y();
    return min;
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
