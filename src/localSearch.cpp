#include <iostream>
#include <climits>
#include <tuple>
#include <math.h>
#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"

const int k = 3;

Polygon_2 localSearch(Points points, string option, double threshold, double &initial_state_area)
{

    // greedy solution
    Polygon_2 initial_state = convex_hull_Algorithm(points, 3);

   initial_state_area = initial_state.area();

    // error checking
    if (!initial_state.is_simple())
    {
        return initial_state;
    }

    // error checking
    if (initial_state.is_empty())
    {
        return initial_state;
    }

    // traverse edges of polygon in random order
    Polygon_2 initial_state_random_order(initial_state);

    std::random_shuffle(initial_state_random_order.begin(), initial_state_random_order.end());

    int max_area = (int)initial_state.area(), current_area, diff = threshold;
    int min_area = (int)initial_state.area();

    Polygon_2 current_polygon(initial_state);
    int counter = 0;
    Polygon_2 old_polygon(initial_state);
    Polygon_2 selected_polygon(initial_state);

    const int max_tries = 20;

    while (diff >= threshold)
    {
        counter++;

        // loop edges
        for (edgeit it = initial_state_random_order.edges_begin(); it != initial_state_random_order.edges_end(); ++it)
        {
            // update polygon with changes
            current_polygon = selected_polygon;
            old_polygon = current_polygon;
            vector<Points> polygon_lines = calculate_polygon_lines(current_polygon);

            bool flag = false;
            for (int i = 0; i < polygon_lines.size(); i++)
            {
                current_polygon = old_polygon;

                Points polygon_line = polygon_lines[i];
                Segment_2 e = *it;

                // we don want polygon lines to contain edge e
                for (int j = 0; j <= polygon_line.size(); j++)
                {
                    if (polygon_line[j] == e[0] || polygon_line[j] == e[1])
                    {
                        flag = true;
                    }
                }

                if (flag)
                {
                    continue;
                }

                // perform move step
                move(current_polygon, polygon_line, *it);

                // check for visibilty
                if (!current_polygon.is_simple())
                {
                    continue;
                }

                current_area = (int)current_polygon.area();
                // error checking
                if (current_area < 0)
                {
                    continue;
                }

                // maximize area
                if (option == "max")
                {
                    if (current_area > max_area)
                    {
                        diff = current_area - max_area;
                        max_area = current_area;
                        selected_polygon = current_polygon;
                    }
                }

                // minimize area
                else
                {
                    if (current_area < max_area)
                    {
                        diff = current_area - max_area;
                        min_area = current_area;
                        selected_polygon = current_polygon;
                    }
                }
            }
        }

        if (counter == max_tries)
        {
            diff = current_area - current_area;
        }
    }
    return selected_polygon;
}