#include <iostream>
#include <climits>
#include <tuple>
#include <initializer_list>

#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"

Polygon_2 localTransitionStep(Polygon_2 polygon, double &alteredArea, int &startingPoint, bool isMax);
Polygon_2 globalTransitionStep(Polygon_2 polygon, double &changeOfPolygonArea, int &pointIndex, int &newPos, bool isMax);

Polygon_2 simulatedAnnealing(Points points, string parameter, bool isMax, int L, double &initial_area)
{
    // cout << "HELLO WORLD SIMULATED ANNEALING!" << endl;
    srand(time(0));

    bool isLocal = string("local").compare(parameter) == 0;

    Polygon_2 polygon = convex_hull_Algorithm(points, 3), convexHull = get_convex_hull_polygon(points);
    // cout << "Area before: " << polygon.area() << endl;
    initial_area = polygon.area();

    double areaOfPolygon = polygon.area(), convexHullArea = convexHull.area();
    double energy = 0, temperature = 1, DE, R = rand() / (RAND_MAX);
    double areaDifference, newEnergy;

    // calculation -> Initial Energy
    energy = points.size() * areaOfPolygon / convexHullArea;

    for (int i = 0; i < L; i++)
    {
        int startingPoint;
        // case of global
        int newPos;

        if (isLocal)
            polygon = localTransitionStep(polygon, areaDifference, startingPoint, isMax);
        else
            polygon = globalTransitionStep(polygon, areaDifference, startingPoint, newPos, isMax);
        // increased Area
        areaOfPolygon += areaDifference;

        // new energy
        newEnergy = points.size() * (areaOfPolygon / convexHullArea);
        DE = newEnergy - energy;

        // check if step is acceptable
        if (isMax)
        {
            // cout << "MAXIMIZATION" << endl;

            if (DE > 0 || exp(-DE / temperature > R))
                energy = newEnergy;
            else
            {
                // if step is not acceptable, undo the step. Based on global.pdf "Check validity. If not valid goto 4."
                if (isLocal)
                    polygon = swapPoints(polygon, startingPoint);

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
                energy = newEnergy;
            else
            {
                // if step is not acceptable, undo the step. Based on global.pdf "Check validity.
                if (isLocal)
                    polygon = swapPoints(polygon, startingPoint);
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

    return polygon;
}

Polygon_2 localTransitionStep(Polygon_2 polygon, double &alteredArea, int &startingPoint, bool isMax)
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
        double deletedArea, addedArea;
        // take a random point in polygon to swap
        int randomPoint = 1 + (rand() % (polygon.size() - 3));
        // q and r are the points to be exchanged. p is previous to q and s is next to r
        Point_2 p = polygon[randomPoint - 1], q = polygon[randomPoint], r = polygon[randomPoint + 1], s = polygon[randomPoint + 2];
        // Create Search Box and get points in box
        Points pointsInBox = get_points_in_box(kdTree, p, q, r, s);
        // Checking for intersects
        bool hasIntersect = checkIntersections(polygon, randomPoint, p, q, r, s, pointsInBox);
        // If we have intersects continue to next iteration
        if (hasIntersect)
            continue;
        // swap points
        polygon = swapPoints(temp_polygon, randomPoint);
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

        deletedArea = polygon.orientation() == triangleOrientation(p, q, r) ? CGAL::area(p, q, r) : CGAL::area(q, r, s);
        addedArea = polygon.orientation() == triangleOrientation(p, q, r) ? CGAL::area(q, r, s) : CGAL::area(p, q, r);
        // Update argumnets passed by reference
        alteredArea = addedArea - deletedArea;
        startingPoint = randomPoint;

        break;
    }

    return polygon;
}

Polygon_2 globalTransitionStep(Polygon_2 polygon, double &changeOfPolygonArea, int &pointIndex, int &newPos, bool isMax)
{
    // cout << "inside global" << endl;
    // temp_polygon uses copy constructor to be initialized as polygon
    Polygon_2 temp_polygon(polygon);

    for (;;)
    {
        int randomPoint = rand() % (polygon.size() - 1);
        pveciterator polygon_start = polygon.begin();

        // cout << "1 - temp polygon area: " << temp_polygon.area() << " polygon area: " << polygon.area() << endl;     
        // pick a point to swap and a new place
        pointIndex = 1 + (rand() % (polygon.size() - 1));
        newPos = 1 + (rand() % (polygon.size() - 1));

        if (abs(pointIndex - newPos) < 2)
            continue;

        Point_2 q = *(polygon_start + pointIndex), r = *(polygon_start + pointIndex - 1), p = *(polygon_start + pointIndex + 1), s = *(polygon_start + newPos - 1), t = *(polygon_start + newPos);

        if (pointIndex < newPos)
            newPos--;

        // make the change
        temp_polygon = pointPositionChange(temp_polygon, pointIndex, newPos);

        // if new polygon is not simple -> return the old one not valid change
        if (temp_polygon.is_simple() == false)
            return polygon;
        else
        {
            if (isMax)
            {
                if (temp_polygon.area() >= polygon.area())
                    polygon = temp_polygon;
                else
                    return polygon;
            }
            else
            {
                if (temp_polygon.area() <= polygon.area())
                    polygon = temp_polygon;
                else
                    return polygon;
            }

            // cout << "5 - temp polygon area: " << temp_polygon.area() << " polygon area: " << polygon.area() << endl;
        }

        // area change
        changeOfPolygonArea = CGAL::area(s, q, t) - CGAL::area(r, q, p);

        break;
    }

    return polygon;
}