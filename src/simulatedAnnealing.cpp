#include <iostream>
#include <climits>
#include <tuple>
#include <initializer_list>

#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"

int triangleOrientation(Point_2 &a, Point_2 &b, Point_2 &c);
int maxCoordinateX(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s);
int maxCoordinateY(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s);
int minCoordinateX(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s);
int minCoordinateY(Point_2 &p, Point_2 &q, Point_2 &r, Point_2 &s);
Points get_points_in_box(Tree &kdTree, Point_2 a, Point_2 b, Point_2 c, Point_2 d);
bool checkIntersections(Polygon_2 &polygon, int randomPointIndex, Point_2 p, Point_2 q, Point_2 r, Point_2 s, Points pointsInBox);
Polygon_2 localTransitionStep(Polygon_2 &polygon, double &alteredArea, int &startingPoint, bool isMax);

void simulatedAnnealing(Points points, string parameter, bool isMax, int L)
{
    cout << "HELLO WORLD SIMULATED ANNEALING!" << endl;
    srand(time(0));

    bool isLocal = string("local").compare(parameter) == 0;

    Polygon_2 polygon = convex_hull_Algorithm(points, 3), convexHull = get_convex_hull_polygon(points);

    // cout << "Area before: " << polygon.area() << endl;

    double areaOfPolygon = polygon.area(), convexHullArea = convexHull.area();
    double energy = 0, temperature = 1, DE, R = rand() / (RAND_MAX);
    double areaDifference, energyAfterStep;

    cout << "Area Before: " << areaOfPolygon << " Simple Polygon: " << polygon.is_simple() << " Area: " << convexHullArea << endl;

    // calculation -> Initial Energy
    energy = points.size() * areaOfPolygon / convexHullArea;

    cout << "Energy: " << energy << endl;

    for (int i = 0; i < L; i++)
    {
        int startingPoint;

        polygon = localTransitionStep(polygon, areaDifference, startingPoint, isMax);
        // calculate new area
        areaOfPolygon += areaDifference;

        // calculate new energy
        energyAfterStep = points.size() * (areaOfPolygon / convexHullArea);
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
                    swapPoints(polygon, startingPoint);
                // calculate new area
                areaOfPolygon -= areaDifference;

                // polygon = localTransitionStep(polygon, areaDifference, indexOfFirstPoint, isMax);
                continue;
            }
        }
        else
        {
            // cout << "MINIMIZATION" << endl;

            if (DE < 0 || exp(-DE / temperature > R))
                energy = energyAfterStep;
            else
            {
                // if step is not acceptable, undo the step. Based on global.pdf "Check validity.
                if (string("local").compare(parameter) == 0)
                    swapPoints(polygon, startingPoint);
                // calculate new area
                areaOfPolygon -= areaDifference;

                // If not valid goto 4."
                continue;
            }
        }

        // reduce temperature
        temperature = temperature - 1 / L;

        if (temperature < 0)
            break;
    }

    cout << "Area after: " << polygon.area() << " Is Simple? " << polygon.is_simple() << endl;
}

Polygon_2 localTransitionStep(Polygon_2 &polygon, double &alteredArea, int &startingPoint, bool isMax)
{
    // temp_polygon uses copy constructor to be initialized as polygon
    Polygon_2 temp_polygon(polygon);

    // initialize kd-Tree
    Tree kdTree;
    for (Point_2 vertex : polygon.vertices())
        kdTree.insert(vertex);
    kdTree.build();

    for (;;)
    {
        // take a random point in polygon to swap
        int randomPoint = 1 + (rand() % (polygon.size() - 3));

        // q and r are the points to be exchanged. p is previous to q and s is next to r
        Point_2 a = polygon[randomPoint - 1], b = polygon[randomPoint], c = polygon[randomPoint + 1], d = polygon[randomPoint + 2];
        // Create Search Box and get points in box
        Points pointsInBox = get_points_in_box(kdTree, a, b, c, d);
        // Checking for intersects
        bool hasIntersect = checkIntersections(polygon, randomPoint, a, b, c, d, pointsInBox);
        // If we have intersects continue to next iteration
        if (hasIntersect)
            continue;
        // swap points
        swapPoints(temp_polygon, randomPoint);
        // different cases for minimization and maximization
        if (isMax)
        {
            if (temp_polygon.area() > polygon.area())
                polygon = temp_polygon;
            else
                return polygon;
        }
        else
        {
            if (temp_polygon.area() < polygon.area())
                polygon = temp_polygon;
            else
                return polygon;
        }
        /**
         * Polygon area will be changed because of point swap
         *  We distinguish two different cases of triangle orientation.
         *  We check if triangle is inside polygon or outside
         **/
        double deletedArea, addedArea;

        deletedArea = polygon.orientation() == triangleOrientation(a, b, c) ? CGAL::area(a, b, c) : CGAL::area(b, c, d);
        addedArea = polygon.orientation() == triangleOrientation(a, b, c) ? CGAL::area(b, c, d) : CGAL::area(a, b, c);

        // Update argumnets passed by reference
        alteredArea = addedArea - deletedArea;
        startingPoint = randomPoint;

        break;
    }

    return polygon;
}