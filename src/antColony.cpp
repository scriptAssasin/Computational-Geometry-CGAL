#include "../include/antColony_helper.hpp"

Polygon_2 antColony(Points points, double alpha, double beta, double ro, bool elitism, int L, string option)
{
    Polygon_2 optimal_polygon;
    Polygon_2 optimal_polygon_in_cycle;
    Polygon_2 current_polygon;

    vector<tuple<Segment_2,int>> trails;

    initialize(points,trails);

    for (int cycle=1; cycle < L+1; cycle++)
    {
        int K = (points.size()/4)+1;
        for (int k=1; k < K; k++)
        {
            current_polygon = build_solution(points,trails,k,option,alpha,beta);
            evaluate_solution(k,current_polygon,optimal_polygon_in_cycle,option);
        }
        SaveBestSolutionSoFar(cycle,current_polygon,optimal_polygon,option);
        update_trails(cycle,elitism,option,trails,ro,K,current_polygon,optimal_polygon_in_cycle);
    }
    
    int area = abs((int)optimal_polygon.area());

    return optimal_polygon;
}
