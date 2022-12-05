#include <iostream>
#include <climits>
#include <tuple>
#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"

const int L = 3;

void getConvexHullPolygonFromPoints(const Points &vertices, Polygon_2 &convexHullPolygon)
{
    Points points;
    CGAL::convex_hull_2(vertices.begin(), vertices.end(), std::back_inserter(points));
    for (auto it = points.begin(); it != points.end(); it++)
        convexHullPolygon.push_back(*it);
}

void simulatedAnnealing(Points points, string parameter)
{
    srand(time(0));
    cout << "HELLO WORLD SIMULATED ANNEALING!" << endl;

    Polygon_2 initial_state = convex_hull_Algorithm(points, 3), convexHullPolygon;
    getConvexHullPolygonFromPoints(initial_state.vertices(), convexHullPolygon);

    cout << "Area before: " << initial_state.area() << endl;

    double energy = 0, temperature = 1, DE, R = rand() / (RAND_MAX);
    double convexHullArea = convexHullPolygon.area();
    double polygonArea = initial_state.area();
    double changeOfPolygonArea, energyAfterStep;

    cout << "Convex Hull Area: " << convexHullArea << endl;

    // calculate energy
    energy = points.size() * polygonArea / convexHullArea;

    for (int i = 0; i < L; i++)
    {
        cout << "iteration: " << L << endl;
    }
}