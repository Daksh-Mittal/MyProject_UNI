#include <set>
#include <vector>
#include <cmath>
#include <iostream>
#include <mcpp/mcpp.h>
#include "models/Waypoints/waypoint.h"
#include "models/PrimAlgorithm/prim.h"
#include "models/Plot/plot.h"
#include "task_c.h"
#include "models/Path/path.h"
#include "config.h"


//Function to connects all waypoints together.
void connect_waypoints(mcpp::Coordinate& start, std::vector<Waypoint>& waypoints, mcpp::MinecraftConnection& mc) {

    //generates min spanning tree using prim's algo
    std::vector<Waypoint> parent = Prim(start, waypoints);
    auto adjacencyList = createAJL(parent, waypoints);

    //keeps track of which edges already connected
    std::set<std::pair<std::pair<int,int>, std::pair<int,int>>> used;

    //loop through the adjacency list and make path between connected waypoints
    for (auto& [key, value] : adjacencyList) {
        mcpp::Coordinate startC(key.first, mc.getHeight(mcpp::Coordinate2D(key.first, key.second)), key.second);

        //loops through all connected nodes
        for (auto& n : value) {
            //makes edge direction independent
            std::pair<int,int> a = {key.first, key.second};
            std::pair<int,int> b = {n.x, n.z};
            if (b < a) std::swap(a, b);

            //skip if already connected
            if (!used.insert({a, b}).second) continue;

            //build path between two waypoints
            Path newPath(startC, {n.x, n.y, n.z});
            breadth_first_search(newPath, mc);
        }
    }
}

//function to seek the nearest waypoint to a plot entrance
Path nearestWaypoint(std::vector<Waypoint>& waypoints, mcpp::Coordinate p) {
    double distance = std::sqrt(std::pow(waypoints[0].x - p.x, 2) +
                     std::pow(waypoints[0].y - p.y, 2) +
                     std::pow(waypoints[0].z - p.z, 2));
    int index = 0;
    for (int i = 1; i < waypoints.size(); ++i) {
        //calculates the distance in terms of blocks from a given waypoint to the plot's entrance
        double temp = std::sqrt(std::pow(waypoints[i].x - p.x, 2) +
                     std::pow(waypoints[i].y - p.y, 2) +
                     std::pow(waypoints[i].z - p.z, 2));
        //finds the closest distance and store the index
        if (temp < distance) {
            distance = temp;
            index = i;
        }
       
    }
    //Use the stored index to create a new path between the plot and the nearest waypoint
    mcpp::Coordinate end(waypoints[index].x, waypoints[index].y, waypoints[index].z);
    Path newPath(p, end);
    return newPath;
}

//function to connect all building entrances to the nearest waypoint
void connect_buildings(std::vector<Waypoint>& waypoints, std::vector<Plot>& p, mcpp::MinecraftConnection& mc) {
    //loop through all the plots, finding the nearest waypoint to the entrance and generating a path
    for (size_t i = 0; i < p.size(); ++i) {
            Path newPath = nearestWaypoint(waypoints, p[i].entrance);
            breadth_first_search(newPath, mc);
    }
};

void testTaskC() {
    int testCase = Config::GetInstance().GetTestCase();
    mcpp::MinecraftConnection mc;
    mcpp::Coordinate origin = mc.getPlayerTilePosition() + mcpp::Coordinate(1, 0, 1);

    std::vector<Plot> testPlots;       //use normal Plot vector (not pointers)
    std::vector<Waypoint> waypoints;

    if (testCase == 1) {
        // Small case
        testPlots.push_back(Plot(origin + mcpp::Coordinate(0, 0, 0),
                                 origin + mcpp::Coordinate(13, 0, 13))); // 14x14
        waypoints.push_back({origin.x + 5, origin.y, origin.z - 5, false});
        waypoints.push_back({origin.x + 20, origin.y, origin.z, false});
    }
    else if (testCase == 2) {
        //Normal test: multiple plots n waypoints
        testPlots.push_back(Plot(origin + mcpp::Coordinate(0, 0, 0),
                                 origin + mcpp::Coordinate(15, 0, 15))); // 16x16
        testPlots.push_back(Plot(origin + mcpp::Coordinate(25, 0, 0),
                                 origin + mcpp::Coordinate(40, 0, 15))); // 2nd plot

        waypoints.push_back({origin.x, origin.y, origin.z, false});
        waypoints.push_back({origin.x + 10, origin.y, origin.z + 20, false});
        waypoints.push_back({origin.x + 35, origin.y, origin.z + 10, false});
    }
    else {
        throw std::invalid_argument("Parameter case must be 1 or 2 for Task C tests");
    }

    std::cout << "Testing component 'pathfinding' with test case " << testCase << std::endl;


    //Connect waypoints
    mcpp::Coordinate start(waypoints.front().x, waypoints.front().y, waypoints.front().z);
    connect_waypoints(start, waypoints, mc);

    //Connect plot to waypoints
    connect_buildings(waypoints, testPlots, mc);
}
