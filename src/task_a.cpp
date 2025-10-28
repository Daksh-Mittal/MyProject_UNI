#include "task_a.h"
#include "config.h"
#include "models/Plot/plot.h"
#include "utils.h"

#include <mcpp/mcpp.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <tuple>
#include <random>

static bool is_in_plot_bounds(const mcpp::Coordinate& coord, const std::vector<Plot>& plots) {
    for (const auto& plot : plots) {
        if (coord.x >= plot.get_min_x() && coord.x <= plot.get_max_x() &&
            coord.z >= plot.get_min_z() && coord.z <= plot.get_max_z()) {
            return true;
        }
    }
    return false;
}

int get_surface_y(mcpp::MinecraftConnection& mc, int x, int z) {
    for (int y = 255; y >= 0; --y) {
        mcpp::Coordinate current_coord(x, y, z);
        mcpp::BlockType block = mc.getBlock(current_coord);
        if (block != mcpp::Blocks::AIR && block != mcpp::Blocks::FLOWING_WATER && block != mcpp::Blocks::STILL_WATER) {
            return y;
        }
    }
    return 63;
}

bool validate_plot(mcpp::MinecraftConnection& mc, const mcpp::Coordinate& origin, 
                   const mcpp::Coordinate& bound, const std::vector<Plot>& existing_plots) {
    const Config& config = Config::GetInstance();
    int plot_border = config.GetPlotBorder();
    
    int plot_min_x = std::min(origin.x, bound.x);
    int plot_max_x = std::max(origin.x, bound.x);
    int plot_min_z = std::min(origin.z, bound.z);
    int plot_max_z = std::max(origin.z, bound.z);
    int plot_area = (plot_max_x - plot_min_x + 1) * (plot_max_z - plot_min_z + 1);
    int min_y = 256;
    int max_y = 0;
    int water_blocks = 0;
    
    Plot proposed_plot(origin, bound);
    for (const auto& existing_plot : existing_plots) {
        if (proposed_plot.is_overlapping(existing_plot)) {
            return false;
        }
    }

    for (int x = plot_min_x; x <= plot_max_x; ++x) {
        for (int z = plot_min_z; z <= plot_max_z; ++z) {
            int y = get_surface_y(mc, x, z);
            
            if (y > max_y) max_y = y;
            if (y < min_y) min_y = y;
            
            mcpp::BlockType block_at_surface = mc.getBlock(mcpp::Coordinate(x, y + 1, z));
            if (block_at_surface == mcpp::Blocks::FLOWING_WATER || block_at_surface == mcpp::Blocks::STILL_WATER) {
                water_blocks++;
            }
        }
    }

    if ((max_y - min_y) > 15) {
        return false;
    }
    
    if ((float)water_blocks / plot_area > 0.15) {
        return false;
    }
    
    const int village_size = config.GetVillageSize();
    mcpp::Coordinate* location = config.GetLocation();
    
    if (location == nullptr) {
        return false; 
    }
    
    const int center_x = location->x;
    const int center_z = location->z;
    
    const int village_min_x = center_x - village_size / 2;
    const int village_max_x = center_x + village_size / 2;
    const int village_min_z = center_z - village_size / 2;
    const int village_max_z = center_z + village_size / 2;
    
    const int border_min_x = plot_min_x - plot_border;
    const int border_max_x = plot_max_x + plot_border;
    const int border_min_z = plot_min_z - plot_border;
    const int border_max_z = plot_max_z + plot_border;
    
    if (border_min_x < village_min_x || border_max_x > village_max_x ||
        border_min_z < village_min_z || border_max_z > village_max_z) {
        return false;
    }
    
    return true;
}

mcpp::Coordinate determine_entrance(const mcpp::Coordinate& origin, const mcpp::Coordinate& bound) {
    const Config& config = Config::GetInstance();
    const int plot_height = origin.y;
    mcpp::Coordinate* location = config.GetLocation();
    
    int center_x = 0;
    int center_z = 0;
    if (location != nullptr) {
        center_x = location->x;
        center_z = location->z;
    }
    
    const int min_x = std::min(origin.x, bound.x);
    const int max_x = std::max(origin.x, bound.x);
    const int min_z = std::min(origin.z, bound.z);
    const int max_z = std::max(origin.z, bound.z);

    const int plot_center_x = (min_x + max_x) / 2;
    const int plot_center_z = (min_z + max_z) / 2;
    
    std::vector<std::pair<int, char>> candidates; 

    if (plot_center_z < center_z) candidates.push_back({std::abs(center_z - min_z), 'Z'}); 
    if (plot_center_z > center_z) candidates.push_back({std::abs(center_z - max_z), 'Z'}); 
    if (plot_center_x < center_x) candidates.push_back({std::abs(center_x - min_x), 'X'}); 
    if (plot_center_x > center_x) candidates.push_back({std::abs(center_x - max_x), 'X'}); 

    if (candidates.empty()) {
        candidates.push_back({std::abs(center_z - min_z), 'Z'});
    }

    std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    char preferred_axis = candidates[0].second;
    int preferred_fixed_coord;
    int range_start, range_end;

    if (preferred_axis == 'Z') {
        if (plot_center_z < center_z) preferred_fixed_coord = min_z;
        else preferred_fixed_coord = max_z;
        range_start = min_x;
        range_end = max_x;
    } else { 
        if (plot_center_x < center_x) preferred_fixed_coord = min_x;
        else preferred_fixed_coord = max_x;
        range_start = min_z;
        range_end = max_z;
    }

    int length = range_end - range_start + 1;
    int door_pos_offset;
    
    if (config.IsTestMode()) {
        door_pos_offset = length / 2; 
    } else {
        int available_range = length - 2;
        if (available_range <= 0) {
            door_pos_offset = 1; 
        } else {
            door_pos_offset = 1 + (std::rand() % available_range); 
        }
    }

    mcpp::Coordinate entrance_coord(0, plot_height + 1, 0); 
    if (preferred_axis == 'Z') {
        entrance_coord.x = range_start + door_pos_offset;
        entrance_coord.z = preferred_fixed_coord;
    } else { 
        entrance_coord.x = preferred_fixed_coord;
        entrance_coord.z = range_start + door_pos_offset;
    }
    
    return entrance_coord;
}


std::vector<Plot> find_plots() {
    mcpp::MinecraftConnection mc; 
    const Config& config = Config::GetInstance();
    std::vector<Plot> plots;
    const int village_size = config.GetVillageSize();
    mcpp::Coordinate* location = config.GetLocation();
    const bool testmode = config.IsTestMode();

    if (location == nullptr) {
        std::cerr << "Error: Village location is not set. Check main.cpp initialization." << std::endl;
        return plots;
    }

    const int center_x = location->x;
    const int center_z = location->z;
    const int village_min_x = center_x - village_size / 2;
    const int village_min_z = center_z - village_size / 2;
    const int village_max_x = center_x + village_size / 2;
    const int village_max_z = center_z + village_size / 2;

    const int required_plots = std::max(1, village_size / 50);
    const int MAX_ATTEMPTS = 1000;
    const int MIN_PLOT_SIZE = 14;
    const int MAX_PLOT_SIZE = 20;
    
    std::srand(config.GetSeed());

    int attempts = 0;
    
    while (attempts < MAX_ATTEMPTS) { 
        int current_plot_size;
        int plot_min_x, plot_min_z;

        if (testmode) {
            const int GRID_STEP = 5;
            
            const int scan_min_x = village_min_x + GRID_STEP;
            const int scan_max_x = village_max_x - GRID_STEP;
            const int scan_min_z = village_min_z + GRID_STEP;
            const int scan_max_z = village_max_z - GRID_STEP;

            int num_x_steps = (scan_max_x - scan_min_x) / GRID_STEP + 1;
            if (num_x_steps < 1) num_x_steps = 1;

            int x_index = attempts % num_x_steps;
            int z_index = attempts / num_x_steps;

            plot_min_x = scan_min_x + x_index * GRID_STEP;
            plot_min_z = scan_min_z + z_index * GRID_STEP;
            
            if (plot_min_x > scan_max_x || plot_min_z > scan_max_z) {
                break;
            }

            current_plot_size = MIN_PLOT_SIZE + (plots.size() % (MAX_PLOT_SIZE - MIN_PLOT_SIZE + 1));
            
        } else {
            current_plot_size = MIN_PLOT_SIZE + (std::rand() % (MAX_PLOT_SIZE - MIN_PLOT_SIZE + 1));
            
            const int max_rand_x = village_max_x - current_plot_size;
            const int max_rand_z = village_max_z - current_plot_size;
            
            if (max_rand_x < village_min_x || max_rand_z < village_min_z) {
                attempts++;
                continue;
            }
            
            plot_min_x = village_min_x + (std::rand() % (max_rand_x - village_min_x + 1));
            plot_min_z = village_min_z + (std::rand() % (max_rand_z - village_min_z + 1));
        }

        const int plot_max_x = plot_min_x + current_plot_size - 1;
        const int plot_max_z = plot_min_z + current_plot_size - 1;

        if (plot_max_x > village_max_x || plot_max_z > village_max_z) {
            attempts++;
            continue;
        }

        int plot_height = 0;
        for (int x = plot_min_x; x <= plot_max_x; ++x) {
            for (int z = plot_min_z; z <= plot_max_z; ++z) {
                plot_height = std::max(plot_height, get_surface_y(mc, x, z));
            }
        }

        mcpp::Coordinate origin(plot_min_x, plot_height, plot_min_z);
        mcpp::Coordinate bound(plot_max_x, plot_height, plot_max_z);

        if (!validate_plot(mc, origin, bound, plots)) {
            attempts++;
            continue;
        }

        mcpp::Coordinate entrance = determine_entrance(origin, bound);

        Plot new_plot(origin, bound, entrance);
        plots.push_back(new_plot);
        
        if (!testmode && (int)plots.size() >= required_plots) { 
            break;
        }

        attempts++;
        if (testmode && plots.size() >= 100) {
            break;
        }
    }

    if ((int)plots.size() < required_plots) {
        std::cerr << "Warning: Could only find " << plots.size() << " plots. Required minimum was " << required_plots << "." << std::endl;
    }

    return plots;
}

void terraform(const std::vector<Plot>& plots) {
    if (plots.empty()) return;

    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    const int border_size = config.GetPlotBorder();
    
    for (const auto& plot : plots) {
        const int plot_height = plot.origin.y;
        const int plot_min_x = plot.get_min_x();
        const int plot_max_x = plot.get_max_x();
        const int plot_min_z = plot.get_min_z();
        const int plot_max_z = plot.get_max_z();
        
        const int terra_min_x = plot_min_x - border_size;
        const int terra_max_x = plot_max_x + border_size;
        const int terra_min_z = plot_min_z - border_size;
        const int terra_max_z = plot_max_z + border_size;

        for (int x = terra_min_x; x <= terra_max_x; ++x) {
            for (int z = terra_min_z; z <= terra_max_z; ++z) {
                
                int dist_x = 0;
                if (x < plot_min_x) dist_x = plot_min_x - x;
                else if (x > plot_max_x) dist_x = x - plot_max_x;
                
                int dist_z = 0;
                if (z < plot_min_z) dist_z = plot_min_z - z;
                else if (z > plot_max_z) dist_z = z - plot_max_z;
                
                const int distance = std::max(dist_x, dist_z);
                
                if (distance == 0) {
                    for (int y = plot_height + 2; y < 256; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
                    }
                    for (int y = 0; y <= plot_height; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::DIRT);
                    }
                    mc.setBlock(mcpp::Coordinate(x, plot_height + 1, z), mcpp::Blocks::GRASS); 
                    
                } else if (distance <= border_size) {
                    const int ground_height = get_surface_y(mc, x, z);
                    
                    const double blend_factor = static_cast<double>(border_size - distance) / border_size;
                    const int new_height = static_cast<int>(std::round(
                        ground_height + (plot_height - ground_height) * blend_factor
                    ));
                    
                    int final_height = std::max(0, std::min(255, new_height));
                    
                    for (int y = final_height + 2; y < 256; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
                    }
                    for (int y = 0; y <= final_height; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::DIRT);
                    }
                    mc.setBlock(mcpp::Coordinate(x, final_height + 1, z), mcpp::Blocks::GRASS); 
                }
            }
        }
    }
}

void fast_build_wall_at(mcpp::MinecraftConnection& mc, int x, int z) {
    const int WALL_HEIGHT = 4;
    const mcpp::BlockType WALL_MATERIAL = mcpp::Blocks::COBBLESTONE;
    const int surface_y = get_surface_y(mc, x, z);
    
    for (int y = surface_y + 1; y < surface_y + WALL_HEIGHT + 2; ++y) {
        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
    }
    
    for (int y = 1; y <= WALL_HEIGHT; ++y) {
        mc.setBlock(mcpp::Coordinate(x, surface_y + y, z), WALL_MATERIAL);
    }
}

void place_wall(const std::vector<Plot>& plots) {
    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    const int village_size = config.GetVillageSize();
    mcpp::Coordinate* location = config.GetLocation();
    
    if (location == nullptr) {
        std::cerr << "Error: Cannot place wall; village location is not set." << std::endl;
        return;
    }

    const int center_x = location->x;
    const int center_z = location->z;
    
    const int village_min_x = center_x - village_size / 2;
    const int village_max_x = center_x + village_size / 2;
    const int village_min_z = center_z - village_size / 2;
    const int village_max_z = center_z + village_size / 2;

    for (int x = village_min_x; x <= village_max_x; ++x) {
        fast_build_wall_at(mc, x, village_min_z);
        fast_build_wall_at(mc, x, village_max_z);
    }
    for (int z = village_min_z + 1; z < village_max_z; ++z) {
        fast_build_wall_at(mc, village_min_x, z);
        fast_build_wall_at(mc, village_max_x, z);
    }
}

// FIX: Corrected return type to std::vector<Waypoint> to match the declaration in task_a.h
std::vector<Waypoint> find_waypoints(const std::vector<Plot>& plots) {
    std::vector<Waypoint> waypoints;
    mcpp::MinecraftConnection mc;
    
    const int required_min_waypoints = std::max(1, (int)plots.size() / 5);
    
    if (plots.empty()) {
        if (required_min_waypoints > 0) {
            std::cerr << "Warning: No plots found, minimum waypoints required: " << required_min_waypoints << "." << std::endl;
        }
        return waypoints;
    }

    long long total_x = 0;
    long long total_z = 0;
    for (const auto& plot : plots) {
        mcpp::Coordinate center = plot.get_center();
        total_x += center.x;
        total_z += center.z;
    }
    
    const int center_x = (int)(total_x / plots.size());
    const int center_z = (int)(total_z / plots.size());
    const int surface_y = get_surface_y(mc, center_x, center_z);
    
    mcpp::Coordinate waypoint_coord(center_x, surface_y + 1, center_z);

    if (!is_in_plot_bounds(waypoint_coord, plots)) {
        waypoints.emplace_back(Waypoint{center_x, surface_y + 1, center_z, false});
    }
    
    if ((int)waypoints.size() < required_min_waypoints) {
        std::cerr << "Warning: Could only find " << waypoints.size() << " waypoints. Required minimum was " << required_min_waypoints << "." << std::endl;
    }

    return waypoints;
}
