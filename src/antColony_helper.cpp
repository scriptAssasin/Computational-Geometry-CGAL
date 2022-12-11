#include "../include/antColony_helper.hpp"

using namespace std;

// initialize ant colony parameters
void initialize(Points points, vector<tuple<Segment_2,int>> &trails)
{
    for (int i=0; i < points.size(); i++)
    {
        for (int j=0; j < points.size(); j++)
        {
            if (points[i] != points[j])
            {
            Segment_2 edge(points[i],points[j]);
            tuple<Segment_2,int> trail;
            int t0 = 1;
            trail = make_tuple(edge,t0);
            // pheromone trails
            trails.push_back(trail);
            }
        }
    }
}

// return feasable nodes for given point
Points feasablePoints(Polygon_2 polygon, Points points) {
    Points feasable_points;
    for (int i=0; i < points.size(); i++)
    {
        for (edgeit it=polygon.edges_begin(); it!=polygon.edges_end(); ++it)
        {
            Polygon_2 temp_pol(polygon);
            update_polygon(*it,points[i],temp_pol);
            // checking visibility and if it is internal
            if (temp_pol.is_simple())
                feasable_points.push_back(points[i]);
        }
    }
    return feasable_points;
}

// return all visible edges for a point p
vector<Segment_2> visibleEdges(Polygon_2 polygon, Point_2 p)
{
    vector<Segment_2> visible_edges;
    for (edgeit it=polygon.edges_begin(); it!=polygon.edges_end(); ++it)
    {
        Polygon_2 temp_pol(polygon);
        update_polygon(*it,p,temp_pol);
        if (temp_pol.is_simple())
        {
            visible_edges.push_back(*it);
        }
    }
    return visible_edges;
}

// select next step based on pheromone probability
Segment_2 selectEdge(Polygon_2 polygon, vector<Segment_2> visible_edges, Point_2 p, vector<tuple<Segment_2,int>> trails, int k, string option, double a, double b)
{
    srand(time(NULL));
    
    vector<tuple<Segment_2,int>> probabilities;
    for (int i=0; i < visible_edges.size(); i++)
    {
        Segment_2 current_edge = visible_edges[i];
        
        tuple<Segment_2,int> edge_ij;
        edge_ij = make_tuple(current_edge,k);

        Polygon_2 temp_pol = polygon;
        update_polygon(current_edge,p,temp_pol);
        

        int t_ij = get<1>(trails[0]);
        int h_ij;
        if (option == "max")
        {
           h_ij = temp_pol.area() - polygon.area();
        }
        else {
            h_ij = 1 / (temp_pol.area() - polygon.area());
        }
         
        int sum;
        for (int j=0; j < visible_edges.size(); j++)
        {
            
            Segment_2 edge = visible_edges[j];
            Polygon_2 pol = polygon;
            update_polygon(edge,p,pol);
            int tij = get<1>(trails[0]);
            int hij;
            if (option == "max")
            {
            hij = (int) (pol.area() - polygon.area());
            }
            else {
                hij = 1 / (pol.area() - polygon.area());
            } 
            sum += pow(tij,a) + pow(hij,b);
        }
        
        int probability = ( (pow(t_ij,a) + pow(h_ij,b)) / sum );
        tuple<Segment_2,int> edge_prob;
        edge_prob = make_tuple(visible_edges[i],probability);
        probabilities.push_back(edge_prob);

        if (temp_pol.is_simple()) {
            return current_edge;
        }

        else {
            return Segment_2(Point_2(0,0),Point_2(0,0));
        }

    }
    int randomindex = rand() % visible_edges.size();
    return visible_edges[randomindex];
}

// ant k that builds a solution (polygonization) to be evaluated
Polygon_2 build_solution(Points initial_points, vector<tuple<Segment_2,int>> trails, int k, string option, double a, double b)
{
    Polygon_2 polygon;
    Points points(initial_points);
    
    //initial triangle
    int randomIndex1 = rand() % points.size();
    int randomIndex2 = rand() % points.size();
    int randomIndex3 = rand() % points.size();
    Point_2 selected_point1 = points[randomIndex1];
    Point_2 selected_point2 = points[randomIndex2];
    Point_2 selected_point3 = points[randomIndex3];
    
    polygon.push_back(selected_point1); 
    polygon.push_back(selected_point2);
    polygon.push_back(selected_point3);

    Points::const_iterator it1 = points.begin()+randomIndex1;
    Points::const_iterator it2 = points.begin()+randomIndex2;
    Points::const_iterator it3 = points.begin()+randomIndex3;
    points.erase(it1);
    points.erase(it2);
    points.erase(it3);

    Polygon_2 old_polygon = polygon;
    
    while (!points.empty())
    {
        old_polygon = polygon;
        Points feasable_points = feasablePoints(polygon,points);
        if (feasable_points.empty()) {return polygon;}

        int randomIndex = rand() % feasable_points.size();
        Point_2 p = feasable_points[randomIndex];

        vector<Segment_2> visible_edges = visibleEdges(polygon,p);

        if (visible_edges.empty()) {return polygon;}

        Segment_2 selected_edge = selectEdge(polygon,visible_edges,p,trails,k,option, a, b);

        // error checking
        if (selected_edge == Segment_2(Point_2(0,0),Point_2(0,0)))
        {return polygon; }
        
        int area = abs((int)polygon.area());
        // expand solution
        update_polygon(selected_edge, p, polygon);
        if (!polygon.is_simple()) {polygon = old_polygon; continue;}

        Points::const_iterator rm;
        
        for (Points::const_iterator it=points.begin(); it!=points.end(); ++it)
        {
            if (*it == p)
            {
                rm = it;
            }
        }
        
        points.erase(rm);  
         
    }
    return polygon;

}



// evaluate solution of k ant
void evaluate_solution(int k, Polygon_2 polygon, Polygon_2 &best_polygon_so_far, string option)
{
    int current_area = abs((int)polygon.area());
    int best_area_so_far = abs((int)best_polygon_so_far.area());
    if (option == "max")
    {
        if (current_area > best_area_so_far)
        {
            best_polygon_so_far = polygon;
        }
    }
    else
    {
       if (current_area < best_area_so_far)
        {
            best_polygon_so_far = polygon;
        } 
    }
}

// evaluate solution from cycle
void SaveBestSolutionSoFar(int cycle, Polygon_2 polygon, Polygon_2 &best_polygon_so_far, string option)
{
    int current_area = abs((int)polygon.area());
    int best_area_so_far = abs((int)best_polygon_so_far.area());
    if (option == "max")
    {
        if (current_area > best_area_so_far)
        {
            best_polygon_so_far = polygon;
        }
    }
    else
    {
       if (current_area < best_area_so_far)
        {
            best_polygon_so_far = polygon;
        } 
    }
}


void update_trails(int cycle, bool elitism, string option, vector<tuple<Segment_2,int>> &trails, double ro, int k, Polygon_2 polygon, Polygon_2 best_solution) 
{
    int pheromone_change;
    if (elitism)
    {
        int sum = 0;
        for (int i=0; i < k; i++)
        {
            int pheromone_k;
            if (option == "max")
            {
                pheromone_k = abs((int)polygon.area());
            }
            else {
                pheromone_k = 1 / (abs((int)polygon.area()));
            }
            sum += pheromone_k;
        }
        pheromone_change = (int)((1/k) * sum); 
    }

    else {
        if (option == "max")
        {
            pheromone_change = best_solution.area();
        }
        else {
            pheromone_change = (int)(1 / best_solution.area());
        }
    }

    for (int i=0; i < trails.size(); i++)
    {
        // new pheromone
        int t_new = ((int)((1-ro) * get<1>(trails[i]))) + pheromone_change;
        tuple<Segment_2,int> new_trail;
        new_trail = make_tuple(get<0>(trails[i]),t_new);
        trails.erase(trails.begin()+i);
        trails.push_back(new_trail);
    }
}