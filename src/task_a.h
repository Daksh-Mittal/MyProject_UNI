#ifndef TASK_A_H
#define TASK_A_H

#include <vector>
#include <mcpp/mcpp.h>
#include "models/Plot/plot.h" 

// Helper functions
int get_surface_y(mcpp::MinecraftConnection& mc, int x, int z);
bool validate_plot(mcpp::MinecraftConnection& mc, const mcpp::Coordinate& origin, 
                   const mcpp::Coordinate& bound, const std::vector<Plot>& existing_plots);
mcpp::Coordinate determine_entrance(const mcpp::Coordinate& origin, const mcpp::Coordinate& bound);
void build_wall_at(mcpp::MinecraftConnection& mc, int x, int z);
void fast_build_wall_at(mcpp::MinecraftConnection& mc, int x, int z);

// Main Task A functions
std::vector<Plot> find_plots();
void terraform(const std::vector<Plot>& plots);
void place_wall(const std::vector<Plot>& plots);
std::vector<mcpp::Coordinate> find_waypoints(const std::vector<Plot>& plots);

#endif


