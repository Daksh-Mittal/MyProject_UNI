#ifndef TASK_C
#define TASK_C
#include <vector>
#include <mcpp/mcpp.h>  // needed for mcpp::Coordinate and MinecraftConnection
#include "models/Plot/plot.h"
#include "models/Waypoints/waypoint.h"
#include "models/Path/path.h"  // correct header name (not "paths.h")

// Connect all waypoints together using prim and bfs
void connect_waypoints(mcpp::Coordinate& start, std::vector<Waypoint>& waypoints, mcpp::MinecraftConnection& mc);

// Connect plot to the nearest waypoints
void connect_buildings(std::vector<Waypoint>& waypoints, std::vector<Plot*>& plots, mcpp::MinecraftConnection& mc);

void testTaskC(const std::string&, int, mcpp::MinecraftConnection&);

//get the coordinate of the entrance one block away from the door
mcpp::Coordinate getEntrance(mcpp::Coordinate entrance, mcpp::MinecraftConnection& mc);

void build_lamp(std::vector<Waypoint>& waypoints, mcpp::MinecraftConnection& mc);
#endif
