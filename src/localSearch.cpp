#include <iostream>
#include <climits>
#include "../include/algorithms.hpp"

typedef Polygon_2::Edge_const_iterator edgeit;
typedef Polygon_2::Vertex_iterator vertexit;

void localSearch(Points points, string parameter, double threshold)
{
    cout << "HELLO WORLD LOCAL SEARCH!" << endl;

    Polygon_2 initial_state = convex_hull_Algorithm(points, 3);

    Polygon_2 initial_state_random_order = initial_state;

    std::random_shuffle(initial_state_random_order.begin(), initial_state_random_order.end());

    // for (vertexit it = initial_state.begin(); it != initial_state.end(); ++it)
    //     std::cout << ' ' << *it << endl;
    double max_area = FLT_MIN, current_area, diff = FLT_MAX;

    while (diff >= threshold)
    {
        for (edgeit it = initial_state_random_order.edges_begin(); it != initial_state_random_order.edges_end(); ++it)
        {
            std::cout << ' ' << *it << endl;
            
            vector <Segment_2, Points> possible_swaps;

            for (vertexit vertit = initial_state.vertices_begin(); it != initial_state.vertices_end(); ++it)
            {
                // swap()... TODO implement swap function. Swap returns new Polygon
                Polygon_2 temp_polygon; // TODO integrate with swap

                if(temp_polygon.area() > max_area)
                {

                }
            }

        }

    }
}