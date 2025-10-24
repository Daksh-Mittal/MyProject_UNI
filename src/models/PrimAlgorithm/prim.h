#ifndef PRIM_H
#define PRIM_H
#include <vector>
#include <map>
#include <mcpp/mcpp.h>
#include "models/Waypoints/waypoint.h"

std::vector<Waypoint> Prim(mcpp::Coordinate start, std::vector<Waypoint> waypoints);
std::map<std::pair<int,int>, std::vector<Waypoint>> createAJL(std::vector<Waypoint>& parent, std::vector<Waypoint>& waypoints);
double calc_dist(int u, int v, const std::vector<Waypoint>& waypoints);
int minimum(std::vector<double>& key, std::vector<bool>& mst);

#endif
