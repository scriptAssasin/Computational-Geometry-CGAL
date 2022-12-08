#include <iostream>
#include <climits>
#include <tuple>
#include <math.h>
#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"

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

const int k = 3; // TODO: move it inside localSearch

void remove_duplicates(Polygon_2 &pol)
{
    for (vertexit vit1 = pol.vertices_begin(); vit1 != pol.vertices_end(); vit1++)
    {
        for (vertexit vit2 = pol.vertices_begin(); vit2 != pol.vertices_end(); vit2++)
        {
            if ((*vit1 == *vit2) && (vit1 != vit2) && (!((vit1 == pol.vertices_begin()) && (vit2 == pol.vertices_end()))) &&
                (!((vit2 == pol.vertices_begin()) && (vit1 == pol.vertices_end()))))
            {
                pol.erase(vit2, next(vit2));
            }
        }
    }
}

Polygon_2 localSearch(Points points, string parameter, double threshold, string option)
{

    // greedy solution
    Polygon_2 initial_state = convex_hull_Algorithm(points, 3);

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