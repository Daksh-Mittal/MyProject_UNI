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

static inline int surfaceYStand(mcpp::MinecraftConnection& mc, int x, int z) {
    int y = mc.getHeight(mcpp::Coordinate2D(x, z));
    auto b = mc.getBlock({x, y, z});
    //checks if the current block is on a tree
    while (b == mcpp::Blocks::OAK_LEAVES || b == mcpp::Blocks::SPRUCE_LEAVES || b == mcpp::Blocks::BIRCH_LEAVES || b == mcpp::Blocks::JUNGLE_LEAVES || b == mcpp::Blocks::ACACIA_LEAVES || b == mcpp::Blocks::OAK_WOOD|| b == mcpp::Blocks::SPRUCE_WOOD || b == mcpp::Blocks::BIRCH_WOOD || b == mcpp::Blocks::JUNGLE_WOOD || b == mcpp::Blocks::ACACIA_WOOD) {
        --y;
        b = mc.getBlock({x, y, z});
    }
    return y + 1;
}

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

//finds the coordinate in front of the door
mcpp::Coordinate getEntrance(mcpp::Coordinate entrance, mcpp::MinecraftConnection& mc) {
    int DX[4] = {1, -1, 0, 0};
    int DZ[4] = {0, 0, 1, -1};

    int x, z;
    x = entrance.x;
    z = entrance.z;
    mcpp::Coordinate ET = entrance;
    for (int i = 0; i < 4; ++i) {
        int nx = x + DX[i];
        int nz = z + DZ[i];
        int ny = mc.getHeight(mcpp::Coordinate2D(nx, nz));

        mcpp::Coordinate checkblock(nx, ny, nz);

        if (mc.getBlock(checkblock) == mcpp::Blocks::GRASS) {

            if (mc.getBlock(mcpp::Coordinate(nx, ny + 1, nz)) == mcpp::Blocks::AIR) {
                ET = checkblock;
                return ET;
            }

        }
    }
    return ET;
}
//builds lamp at each waypoints
void build_lamp(std::vector<Waypoint>& wps, mcpp::MinecraftConnection& mc) {
    for (auto& wp : wps) {
        int y = surfaceYStand(mc, wp.x, wp.z);
        auto b = mc.getBlock({wp.x, y, wp.z});
        while (b == mcpp::Blocks::OAK_FENCE || b == mcpp::Blocks::GLOWSTONE) {
            --y;
            b = mc.getBlock({wp.x, y, wp.z});
        }
        mc.setBlock({wp.x, y, wp.z}, mcpp::Blocks::GRAVEL);

        for (int h = 0; h < 3; ++h) {
            mc.setBlock({wp.x, y + 1 + h, wp.z}, mcpp::Blocks::OAK_FENCE);
        }
        mc.setBlock({wp.x, y + 4, wp.z}, mcpp::Blocks::GLOWSTONE);
    }
}


//function to connect all building entrances to the nearest waypoint
void connect_buildings(std::vector<Waypoint>& waypoints, std::vector<Plot*>& p, mcpp::MinecraftConnection& mc) {
    //loop through all the plots, finding the nearest waypoint to the entrance and generating a path

    for (size_t i = 0; i < p.size(); ++i) {
        mcpp::Coordinate entrance = getEntrance(p[i]->entrance, mc);
        Path newPath = nearestWaypoint(waypoints, entrance);
        breadth_first_search(newPath, mc);
    }
};


void testTaskC(const std::string& component, int testCase, mcpp::MinecraftConnection& mc) {
    mcpp::Coordinate origin = mc.getPlayerTilePosition() + mcpp::Coordinate(1, 0, 1);
    std::vector<Plot*> testPlots; 
    std::vector<Waypoint> waypoints;

    if (testCase == 1) {
        std::cout << "Task_C test case 1: connecting 4 waypoints" << std::endl;
        waypoints.push_back({200, 69, 200, false});
        waypoints.push_back({211, 68, 204, false});
        waypoints.push_back({193, 70, 211, false});
        waypoints.push_back({215, 67, 232, false});
        mcpp::Coordinate start(waypoints.front().x, waypoints.front().y, waypoints.front().z);
        connect_waypoints(start, waypoints, mc);
        build_lamp(waypoints, mc);
    }
    else if (testCase == 2) {
        //Normal test: multiple plots n waypoints
        testPlots.push_back(new Plot(origin + mcpp::Coordinate(0, 0, 0),
                                 origin + mcpp::Coordinate(15, 0, 15))); // 16x16
        testPlots.push_back(new Plot(origin + mcpp::Coordinate(25, 0, 0),
                                 origin + mcpp::Coordinate(40, 0, 15))); // 2nd plot

        waypoints.push_back({origin.x, origin.y, origin.z, false});
        waypoints.push_back({origin.x + 10, origin.y, origin.z + 20, false});
        waypoints.push_back({origin.x + 35, origin.y, origin.z + 10, false});
    }
    else {
        throw std::invalid_argument("Parameter case must be 1 or 2 for Task C tests");
    }
    for (auto& testPlot : testPlots) {
        delete testPlot;
    }
}
