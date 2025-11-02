#include "models/Waypoints/waypoint.h"
#include "models/PrimAlgorithm/prim.h"
#include <mcpp/mcpp.h>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include <climits>

//calculates. the distance between two coordinates
double calc_dist(int u, int v, const std::vector<Waypoint>& waypoints) {
    const Waypoint goal = waypoints.at(v);
    const Waypoint start = waypoints.at(u);
    return std::sqrt(std::pow(goal.x - start.x, 2) +
                     std::pow(goal.y - start.y, 2) +
                     std::pow(goal.z - start.z, 2));
}

//finds the vertex with the min value thats not in the MST yet
int minimum(std::vector<double>& key, std::vector<bool>& mst) {
    double min = INT_MAX;
    int min_index = -1;
    for (size_t i = 0; i < mst.size(); i++) {
        //store node if it is not in tree and has smaller key
        if (!mst[i] && key[i] < min) {
            min = key[i];
            min_index = i;
        }
    }
    return min_index;
}

//Prim's algorithm, builds the minimum spanning tree
std::vector<Waypoint> Prim(mcpp::Coordinate start, std::vector<Waypoint> waypoints) {
    const int v = waypoints.size();
    std::vector<bool> mstSet(v, false);
    std::vector<double> key(v);
    std::vector<Waypoint> parent(v);

    //initate all the keys to as large int as possible and set all mst to be false
    for (int i = 0; i < v; ++i) {
        key[i] = INT_MAX;
        mstSet[i] = false;
    }

    //start from 1st waypoint
    key[0] = 0;
    parent[0].hasParent = false; //set it to haev not parent as it is the root

    //main prim algo loop
    //repeat till all vertex are processed
    for (int i = 0; i < v - 1; ++i) {
        int u = minimum(key, mstSet); //picks the vertext with the smallest key by calling the minimum function
        if (u < 0) break; //if its invalid, then stop
        mstSet[u] = true;

        //check all other vertexs and update if shorter connection found
        for (int j = 0; j < v; ++j) {
            double distance = calc_dist(u, j, waypoints);
            if (!mstSet[j] && distance < key[j]) {
                parent[j] = waypoints[u];
                parent[j].hasParent = true;
                key[j] = distance;
            }
        }
    }
    return parent;
}

//builds adjacency list from parent array, basically connects each child waypoint to its parent
std::map<std::pair<int,int>, std::vector<Waypoint>> createAJL(std::vector<Waypoint>& parent, std::vector<Waypoint>& waypoints) {

    std::map<std::pair<int,int>, std::vector<Waypoint>> adj;

    for (size_t i = 0; i < parent.size(); ++i) {
        if (i == 0) continue;
        Waypoint p = parent[i];
        Waypoint c = waypoints[i];
        adj[{p.x,p.z}].push_back(c);
        adj[{c.x,c.z}].push_back(p);
    }
    return adj;
}
