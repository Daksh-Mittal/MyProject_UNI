#ifndef PATHS_H
#define PATHS_H
#include "models/Waypoints/waypoint.h"
#include <mcpp/mcpp.h>

//define path structure
struct Path {
  mcpp::Coordinate start;
  mcpp::Coordinate end;

  Path(mcpp::Coordinate start, mcpp::Coordinate end);
};

void breadth_first_search(Path& newPath, mcpp::MinecraftConnection& mc);
bool** createVisited(int, int);

#endif

