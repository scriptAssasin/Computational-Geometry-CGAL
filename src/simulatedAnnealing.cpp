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

const int L = 10000;

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

Polygon_2 localTransitionStep(Polygon_2 &polygon, double &changeOfPolygonArea, int &indexOfFirstPoint, bool isMax)
{
    Polygon_2 temp_polygon(polygon);
    // create kd-Tree
    Tree kdTree;
    for (Point_2 vertex : polygon.vertices())
        kdTree.insert(vertex);
    kdTree.build();

    bool validPointSwap;

    while (1)
    {

        // take a random point in polygon to swap
        int randomPointIndex = 1 + (rand() % (polygon.size() - 3));

        Point_2 p, q, r, s; // q and r are the points to be exchanged. p is previous to q and s is next to r
        p = polygon[randomPointIndex - 1];
        q = polygon[randomPointIndex];
        r = polygon[randomPointIndex + 1];
        s = polygon[randomPointIndex + 2];

        // find max and min coordianates to form the search box
        int maxX = maxCoordinateX(p, q, r, s);
        int minX = minCoordinateX(p, q, r, s);
        int maxY = maxCoordinateY(p, q, r, s);
        int minY = minCoordinateY(p, q, r, s);

        // std::cout << '[' << p.x() << ',' << p.y() << "]," << '[' << q.x() << ',' << q.y() << "]," << '[' << r.x() << ',' << r.y() << "]," << '[' << s.x() << ',' << s.y() << "]," << '[' << minX << ',' << minY << ']' << std::endl;
        // std::cout << "box = [ " <<  '[' << minX << ',' << minY << "]," << '[' << maxX << ',' << minY << "]," << '[' << maxX << ',' << maxY << "]," << '[' << minX << ',' << maxY << "]," << '[' << minX << ',' << minY << "] ]" << std::endl;
        // utils::polygonToPythonArray(polygon);

        // find the points of polygon in the box
        std::vector<Point_2> pointsInBox;
        Fuzzy_iso_box searchBox(Point_2(minX, minY), Point_2(maxX, maxY));
        kdTree.search(std::back_inserter(pointsInBox), searchBox);

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
            continue;

        // checks if the line after s intersects with line p r
        if (randomPointIndex + 3 < polygon.size())
            t = polygon[randomPointIndex + 3];
        else
            t = polygon[0]; // if s is the last point in polygon
        lineT = Segment_2(s, t);
        if (CGAL::do_intersect(line1, lineT))
            continue;

        // if new lines intersect each other try again
        if (CGAL::do_intersect(line1, line2))
            continue;

        // check if lines from points in box intersect with new lines
        for (Polygon_2::Vertex_iterator vertex = polygon.begin(); vertex != polygon.end(); vertex++)
        {

            validPointSwap = true;

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
                        validPointSwap = false;

                    break;
                }
            }

            if (!validPointSwap)
                break;
        }

        // if a line intersect with one of the new lines, try again
        if (!validPointSwap)
            continue;

        // utils::polygonToPythonArray(polygon);

        // cout << "inside local transition step" << endl;

        // cout << "Before Swap: " << polygon.area() << endl;
        // swap points
        swapTwoPoints(temp_polygon, randomPointIndex);

        if (isMax)
        {
            if (temp_polygon.area() > polygon.area())
            {
                polygon = temp_polygon;
            }
            else
            {
                return polygon;
            }
        }
        else
        {
            if (temp_polygon.area() < polygon.area())
            {
                polygon = temp_polygon;
            }
            else
            {
                return polygon;
            }
        }

        // cout << "After Swap: " << polygon.area() << endl;
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

        indexOfFirstPoint = randomPointIndex;

        // utils::printOutput2(polygon, pointsInBox, p, q, r, s);
        //  if(!polygon.is_simple()){
        //  	std::cout << "Not ok " << randomPointIndex << "\n";
        //  	exit(1);
        //  }
        //  else

        break;
    }

    return polygon;
}

void simulatedAnnealing(Points points, string parameter, bool isMax)
{
    cout << "HELLO WORLD SIMULATED ANNEALING!" << endl;
    srand(time(0));

    Polygon_2 polygon = convex_hull_Algorithm(points, 3), convexHull = get_convex_hull_polygon(points);

    // cout << "Area before: " << polygon.area() << endl;

    double polygonArea = polygon.area(), convexHullArea = convexHull.area();
    double energy = 0, temperature = 1, DE, R = rand() / (RAND_MAX);
    double changeOfPolygonArea, energyAfterStep;

    cout << "Area Before: " << polygonArea << " Simple Polygon: " << polygon.is_simple() << " Area: " << convexHullArea << endl;

    // calculation -> Initial Energy
    energy = points.size() * polygonArea / convexHullArea;

    cout << "Energy: " << energy << endl;

    for (int i = 0; i < L; i++)
    {
        int indexOfFirstPoint;

        // Local Transition Step will return the index of first point that will be swapped
        // indexOfFirstPoint = localTransitionStep(polygon, changeOfPolygonArea);

        polygon = localTransitionStep(polygon, changeOfPolygonArea, indexOfFirstPoint, isMax);
        // calculate new area
        polygonArea += changeOfPolygonArea;

        // calculate new energy
        energyAfterStep = points.size() * (polygonArea / convexHullArea);
        DE = energyAfterStep - energy;

        // check if step is acceptable
        if (isMax)
        {
            // cout << "MAXIMIZATION" << endl;

            if (DE > 0 || exp(-DE / temperature > R))
                energy = energyAfterStep;
            else
            {
                // if step is not acceptable, undo the step. Based on global.pdf "Check validity. If not valid goto 4."
                if (string("local").compare(parameter) == 0)
                    swapTwoPoints(polygon, indexOfFirstPoint);

                polygon = localTransitionStep(polygon, changeOfPolygonArea, indexOfFirstPoint, isMax);
                // calculate new area
                polygonArea += changeOfPolygonArea;
            }
        }
        else
        {
            // cout << "MINIMIZATION" << endl;

            if (DE < 0 || exp(-DE / temperature > R))
                energy = energyAfterStep;
            else
            {
                // if step is not acceptable, undo the step. Based on global.pdf "Check validity. If not valid goto 4."
                if (string("local").compare(parameter) == 0)
                    swapTwoPoints(polygon, indexOfFirstPoint);

                localTransitionStep(polygon, changeOfPolygonArea, indexOfFirstPoint, isMax);

                // calculate new area
                polygonArea -= changeOfPolygonArea;
            }
        }

        // reduce temperature
        temperature = temperature - 1 / L;

        if (temperature < 0)
        {
            break;
        }
    }

    cout << "Area after: " << polygon.area() << " Is Simple? " << polygon.is_simple() << endl;
}
