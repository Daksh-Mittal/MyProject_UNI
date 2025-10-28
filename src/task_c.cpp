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


void connect_waypoints(mcpp::Coordinate& start, std::vector<Waypoint>& waypoints, mcpp::MinecraftConnection& mc) {

    std::vector<Waypoint> parent = Prim(start, waypoints);
    auto adjacencyList = createAJL(parent, waypoints);

    std::set<std::pair<std::pair<int,int>, std::pair<int,int>>> used;

    for (auto& [key, value] : adjacencyList) {
        // FIX 1: Pass X and Z separately to mc.getHeight (assumes mc.getHeight(int x, int z) signature)
        mcpp::Coordinate startC(key.first, mc.getHeight(key.first, key.second), key.second);

        for (auto& n : value) {
            std::pair<int,int> a = {key.first, key.second};
            std::pair<int,int> b = {n.x, n.z};
            if (b < a) std::swap(a, b);

            if (!used.insert({a, b}).second) continue;

            // FIX 2: Explicitly call mcpp::Coordinate constructor instead of using initializer list
            Path newPath(startC, mcpp::Coordinate(n.x, n.y, n.z));
            breadth_first_search(newPath, mc);
        }
    }
}

Path nearestWaypoint(std::vector<Waypoint>& waypoints, mcpp::Coordinate p) {
    double distance = std::sqrt(std::pow(waypoints[0].x - p.x, 2) +
                     std::pow(waypoints[0].y - p.y, 2) +
                     std::pow(waypoints[0].z - p.z, 2));
    int index = 0;
    // FIX 3: Use size_t for loop counter to avoid signed/unsigned comparison error
    for (size_t i = 1; i < waypoints.size(); ++i) {
        double temp = std::sqrt(std::pow(waypoints[i].x - p.x, 2) +
                     std::pow(waypoints[i].y - p.y, 2) +
                     std::pow(waypoints[i].z - p.z, 2));
        if (temp < distance) {
            distance = temp;
            index = i;
        }
       
    }
    mcpp::Coordinate end(waypoints[index].x, waypoints[index].y, waypoints[index].z);
    Path newPath(p, end);
    return newPath;
}

void connect_buildings(std::vector<Waypoint>& waypoints, std::vector<Plot*>& p, mcpp::MinecraftConnection& mc) {
    for (size_t i = 0; i < p.size(); ++i) {
            Path newPath = nearestWaypoint(waypoints, p[i]->entrance);
            breadth_first_search(newPath, mc);
    }
}

void testTaskC(const std::string& component, int testCase, mcpp::MinecraftConnection& mc) {
    mcpp::Coordinate origin = mc.getPlayerTilePosition() + mcpp::Coordinate(1, 0, 1);

    std::vector<Plot*> testPlots;
    std::vector<Waypoint> waypoints;

    if (testCase == 1) {
        testPlots.push_back(new Plot(origin + mcpp::Coordinate(0, 0, 0),
                                 origin + mcpp::Coordinate(13, 0, 13)));
        waypoints.push_back({origin.x + 5, origin.y, origin.z - 5, false});
        waypoints.push_back({origin.x + 20, origin.y, origin.z, false});
    }
    else if (testCase == 2) {
        testPlots.push_back(new Plot(origin + mcpp::Coordinate(0, 0, 0),
                                 origin + mcpp::Coordinate(15, 0, 15)));
        testPlots.push_back(new Plot(origin + mcpp::Coordinate(25, 0, 0),
                                 origin + mcpp::Coordinate(40, 0, 15)));

        waypoints.push_back({origin.x, origin.y, origin.z, false});
        waypoints.push_back({origin.x + 10, origin.y, origin.z + 20, false});
        waypoints.push_back({origin.x + 35, origin.y, origin.z + 10, false});
    }
    else {
        throw std::invalid_argument("Parameter case must be 1 or 2 for Task C tests");
    }

    std::cout << "Testing component 'pathfinding' with test case " << testCase << std::endl;


    mcpp::Coordinate start(waypoints.front().x, waypoints.front().y, waypoints.front().z);
    connect_waypoints(start, waypoints, mc);

    connect_buildings(waypoints, testPlots, mc);

    for (auto& testPlot : testPlots) {
        delete testPlot;
    }
}
