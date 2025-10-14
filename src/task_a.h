#ifndef TASK_A
#define TASK_A

#include <vector>
#include <mcpp/mcpp.h>
#include "models/Plot/plot.h" 

int get_surface_y(mcpp::MinecraftConnection& mc, int x, int z) ;

bool validate_plot(mcpp::MinecraftConnection& mc, const mcpp::Coordinate& origin, const mcpp::Coordinate& bound, const std::vector<Plot>& existing_plots) ;

mcpp::Coordinate determine_entrance(const mcpp::Coordinate& origin, const mcpp::Coordinate& bound) ;

std::vector<mcpp::Coordinate> find_waypoints(const std::vector<Plot>& plots) ;

std::vector<Plot> find_plots() ;
void terraform(const std::vector<Plot>& plots) ;
void place_wall() ;

#endif

