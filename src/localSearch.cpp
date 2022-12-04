#include <iostream>
#include <climits>
#include <tuple>
#include "../include/algorithms.hpp"
#include "../include/polygonizationHelper.hpp"

void printtVector(vector<Point_2> vec)
{
    for (int i = 0; i < vec.size(); i++)
        cout << vec[i].x() << " " << vec[i].y() << endl;
}

bool is_visible_path(Polygon_2 &polygon, Points path, Segment_2 edge)
{
    for (int i = 0; i < path.size(); i++)
    {
        if (!is_visible(polygon, path[i], edge))
        {
            return false;
        }
    }

    return true;
}

Polygon_2 move(Polygon_2 &starting_polygon, Points polygon_line, Segment_2 edge)
{
    Polygon_2 final_polygon;

    return final_polygon;
}

vector<Points> calculate_polygon_lines(Polygon_2 &polygon)
{

    vector<Points> polygon_lines;

    for (vertexit vertit = polygon.vertices_begin(); vertit != polygon.vertices_end(); ++vertit)
    {
        Points polygon_line;
        // vertexit temp = vertit;

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

        // cout << "------------------------" << endl;
        // printtVector(polygon_line);
        // cout << "------------------------" << endl;

        polygon_lines.push_back(polygon_line);
        // cout << *vertit << endl;
    }

    return polygon_lines;
}

const int k = 3; // TODO: move it inside localSearch

void localSearch(Points points, string parameter, double threshold)
{
    cout << "HELLO WORLD LOCAL SEARCH!" << endl;

    Polygon_2 initial_state = convex_hull_Algorithm(points, 3);

    /** TODO: TO DELETE STARTS HERE **/

    // /** PRINT POLYGON VERTICES **/
    // cout << "-------------------" << endl;
    // for (vertexit vertit = initial_state.vertices_begin(); vertit != initial_state.vertices_end(); ++vertit)
    // {
    //     cout << *vertit << endl;
    // }
    // cout << "-------------------" << endl;
    // /** PRINT POLYGON EDGES **/
    // for (edgeit iter = initial_state.edges_begin(); iter != initial_state.edges_end(); ++iter)
    // {
    //     cout << *iter << endl;
    // }

    // cout << "-------------------" << endl;

    /** TODO: TO DELETE ENDS HERE **/

    Polygon_2 initial_state_random_order = initial_state;

    std::random_shuffle(initial_state_random_order.begin(), initial_state_random_order.end());

    // for (vertexit it = initial_state.begin(); it != initial_state.end(); ++it)
    //     std::cout << ' ' << *it << endl;
    double max_area = FLT_MIN, current_area, diff = FLT_MAX;
    while (diff >= threshold)
    {
        Polygon_2 current_polygon = initial_state; // TODO: integrate with swap

        for (edgeit it = initial_state_random_order.edges_begin(); it != initial_state_random_order.edges_end(); ++it)
        {
            // std::cout << ' ' << *it << endl;

            vector<tuple<Segment_2, Points>> swaps;
            vector<Points> polygon_lines = calculate_polygon_lines(current_polygon);

            // for (vertexit vertit = initial_state.vertices_begin(); vertit != initial_state.vertices_end(); ++vertit)
            // {
            //     // swap()... TODO: implement swap function. Swap returns new Polygon
            // }

            for (int i = 0; i < polygon_lines.size(); i++)
            {
                Polygon_2 temp_polygon;
                Points polygon_line = polygon_lines[i];
                if (is_visible_path(current_polygon, polygon_line, *it))
                {
                    temp_polygon = move(current_polygon, polygon_line, *it);

                    if (temp_polygon.area() > max_area)
                    {
                        if(!temp_polygon.is_simple())
                        {
                            cout << "BACKTRACKING TO OPTIMIZE AREA!" << endl;
                        }
                        current_polygon = temp_polygon;
                    }
                }
            }
        }

        cout << "Is Simple??? " << current_polygon.is_simple() << endl;
        break;
    }
}