#include <iostream>

#include "../include/algorithms.hpp"

void localSearch(Points points, string parameter, double threshold)
{
    Polygon_2 initial_state = convex_hull_Algorithm(points, 3);
    cout << "HELLO WORLD LOCAL SEARCH!" << endl;
}