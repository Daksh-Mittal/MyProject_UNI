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
        if (block != mcpp::Blocks::AIR && block != mcpp::BlockType(8) && block != mcpp::BlockType(9)) {
            return y;
        }
    }
    return 63;
}

bool validate_plot(mcpp::MinecraftConnection& mc, const mcpp::Coordinate& origin, const mcpp::Coordinate& bound, const std::vector<Plot>& existing_plots) {
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

    std::cout << "DEBUG: Validating plot at (" << plot_min_x << "," << plot_min_z << ") to (" 
              << plot_max_x << "," << plot_max_z << ")" << std::endl;

    for (int x = plot_min_x; x <= plot_max_x; ++x) {
        for (int z = plot_min_z; z <= plot_max_z; ++z) {
            int y = get_surface_y(mc, x, z);
            if (y > max_y) max_y = y;
            if (y < min_y) min_y = y;
            
            mcpp::BlockType block = mc.getBlock(mcpp::Coordinate(x, y + 1, z));
            if (block == mcpp::BlockType(8) || block == mcpp::BlockType(9)) water_blocks++;
        }
    }

    int slope_delta = max_y - min_y;
    if (slope_delta > 15) {
        std::cout << "DEBUG: REJECTED - Slope too steep: " << slope_delta << std::endl;
        return false;
    }
    
    float current_water_coverage = (float)water_blocks / plot_area;
    if (current_water_coverage > 0.15) {
        std::cout << "DEBUG: REJECTED - Too much water: " << current_water_coverage << std::endl;
        return false;
    }
    
    const int village_size = config.GetVillageSize();
    mcpp::Coordinate* location = config.GetLocation(); 
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
        std::cout << "DEBUG: REJECTED - Border exceeds village boundary" << std::endl;
        return false;
    }
    
    std::cout << "DEBUG: ACCEPTED - Plot is valid" << std::endl;
    return true;
}

mcpp::Coordinate determine_entrance(const mcpp::Coordinate& origin, const mcpp::Coordinate& bound) {
    const Config& config = Config::GetInstance();
    const int min_x = std::min(origin.x, bound.x);
    const int max_x = std::max(origin.x, bound.x);
    const int min_z = std::min(origin.z, bound.z);
    const int max_z = std::max(origin.z, bound.z);
    const int plot_height = origin.y;
    mcpp::Coordinate* location = config.GetLocation(); 
    const int center_x = location->x;
    const int center_z = location->z;

    const int dist_to_bottom_edge = std::abs(center_z - min_z);
    const int dist_to_top_edge = std::abs(center_z - max_z);
    const int dist_to_left_edge = std::abs(center_x - min_x);
    const int dist_to_right_edge = std::abs(center_x - max_x);

    std::tuple<int, int, int, int, int, char> sides[4] = {
        {dist_to_bottom_edge, 0, min_x, min_z, max_x - min_x, 'X'},
        {dist_to_top_edge, 1, min_x, max_z, max_x - min_x, 'X'},
        {dist_to_left_edge, 2, min_z, min_x, max_z - min_z, 'Z'},
        {dist_to_right_edge, 3, min_z, max_x, max_z - min_z, 'Z'}
    };

    std::sort(sides, sides + 4, [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    const int start_coord = std::get<2>(sides[0]);
    const int fixed_coord = std::get<3>(sides[0]);
    const int length = std::get<4>(sides[0]);
    const char axis = std::get<5>(sides[0]);
    
    int door_pos_offset;
    if (config.IsTestMode()) {
        door_pos_offset = length / 2;
    } else {
        int available_range = length - 1;
        if (available_range <= 1) {
            door_pos_offset = 1;
        } else {
            door_pos_offset = 1 + (std::rand() % (available_range - 1)); 
        }
    }

    mcpp::Coordinate entrance_coord(0, plot_height + 1, 0); 
    if (axis == 'X') {
        entrance_coord.x = start_coord + door_pos_offset;
        entrance_coord.z = fixed_coord;
    } else { 
        entrance_coord.x = fixed_coord;
        entrance_coord.z = start_coord + door_pos_offset;
    }
    return entrance_coord;
}


std::vector<Plot> find_plots() {
    mcpp::MinecraftConnection mc; 
    const Config& config = Config::GetInstance();
    std::vector<Plot> plots;
    const int village_size = config.GetVillageSize();
    mcpp::Coordinate* location = config.GetLocation();
    const int center_x = location->x;
    const int center_z = location->z;
    const bool testmode = config.IsTestMode();

    std::cout << "DEBUG: Finding plots around (" << center_x << "," << center_z 
              << ") with size " << village_size << std::endl;

    const int village_min_x = center_x - village_size / 2;
    const int village_min_z = center_z - village_size / 2;
    const int village_max_x = center_x + village_size / 2;
    const int village_max_z = center_z + village_size / 2;

    std::cout << "DEBUG: Village bounds: X[" << village_min_x << "-" << village_max_x 
              << "] Z[" << village_min_z << "-" << village_max_z << "]" << std::endl;

    const int required_plots = std::max(1, village_size / 50);
    const int MAX_ATTEMPTS = 1000;
    const int MIN_PLOT_SIZE = 14;
    const int MAX_PLOT_SIZE = 20;
    
    std::srand(config.GetSeed());

    int attempts = 0;
    int plot_size_cycle = MIN_PLOT_SIZE;

    while (attempts < MAX_ATTEMPTS) { 
        int current_plot_size;
        int plot_min_x, plot_min_z;

        if (testmode) {
            const int grid_size = 5;
            const int grid_cells_x = (village_max_x - village_min_x) / grid_size; 
            
            const int scan_x_index = (attempts * grid_size) % (grid_cells_x * grid_size + grid_size);
            const int scan_z_index = (attempts * grid_size) / (grid_cells_x * grid_size + grid_size) * grid_size;

            plot_min_x = village_min_x + scan_x_index;
            plot_min_z = village_min_z + scan_z_index;

            current_plot_size = plot_size_cycle;
            plot_size_cycle = (plot_size_cycle == MAX_PLOT_SIZE) ? MIN_PLOT_SIZE : plot_size_cycle + 1;
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

        mcpp::Coordinate origin(plot_min_x, 0, plot_min_z);
        mcpp::Coordinate bound(plot_max_x, 0, plot_max_z);

        if (!validate_plot(mc, origin, bound, plots)) {
            attempts++;
            continue;
        }

        int plot_height = 0;
        for (int x = plot_min_x; x <= plot_max_x; ++x) {
            for (int z = plot_min_z; z <= plot_max_z; ++z) {
                plot_height = std::max(plot_height, get_surface_y(mc, x, z));
            }
        }

        origin.y = plot_height;
        bound.y = plot_height;

        Plot proposed_plot(origin, bound); 
        bool intersects = false;
        for (const auto& existing_plot : plots) {
            if (proposed_plot.is_overlapping(existing_plot)) {
                intersects = true;
                break;
            }
        }
        if (intersects) {
            attempts++;
            continue;
        }

        mcpp::Coordinate entrance = determine_entrance(origin, bound);

        Plot new_plot(origin, bound);
        new_plot.entrance = entrance;
        new_plot.buildingHeight = plot_height; 

        plots.push_back(new_plot);
        std::cout << "DEBUG: Added plot #" << plots.size() << " at (" << plot_min_x << "," << plot_min_z 
                  << ") size " << current_plot_size << " height " << plot_height << std::endl;

        if (!testmode && (int)plots.size() >= required_plots) {
            break;
        }

        attempts++;
    }

    if ((int)plots.size() < required_plots) {
        std::cerr << "Warning: Could only find " << plots.size() << " plots. Required minimum was " << required_plots << "." << std::endl;
    }

    std::cout << "DEBUG: Found " << plots.size() << " plots (required: " << required_plots << ")" << std::endl;
    return plots;
}

void terraform(const std::vector<Plot>& plots) {
    if (plots.empty()) {
        std::cout << "DEBUG: No plots to terraform" << std::endl;
        return;
    }

    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    const int border_size = config.GetPlotBorder();
    
    std::cout << "DEBUG: Starting terraforming for " << plots.size() << " plots" << std::endl;

    for (const auto& plot : plots) {
        const int plot_height = plot.origin.y;
        const int plot_min_x = std::min(plot.origin.x, plot.bound.x);
        const int plot_max_x = std::max(plot.origin.x, plot.bound.x);
        const int plot_min_z = std::min(plot.origin.z, plot.bound.z);
        const int plot_max_z = std::max(plot.origin.z, plot.bound.z);
        
        std::cout << "DEBUG: Terraforming plot at (" << plot_min_x << "," << plot_min_z 
                  << ") height " << plot_height << std::endl;

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
    std::cout << "DEBUG: Terraforming completed" << std::endl;
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

void build_wall_at(mcpp::MinecraftConnection& mc, int x, int z) {
    fast_build_wall_at(mc, x, z); 
}

void place_wall(const std::vector<Plot>& plots) {
    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    const int village_size = config.GetVillageSize();
    mcpp::Coordinate* location = config.GetLocation();
    const int center_x = location->x;
    const int center_z = location->z;
    
    std::cout << "DEBUG: Building wall around (" << center_x << "," << center_z 
              << ") size " << village_size << std::endl;

    const int village_min_x = center_x - village_size / 2;
    const int village_max_x = center_x + village_size / 2;
    const int village_min_z = center_z - village_size / 2;
    const int village_max_z = center_z + village_size / 2;

    std::cout << "\t Building walls..." << std::endl;

    for (int x = village_min_x; x <= village_max_x; ++x) {
        fast_build_wall_at(mc, x, village_min_z);
        fast_build_wall_at(mc, x, village_max_z);
    }
    for (int z = village_min_z + 1; z < village_max_z; ++z) {
        fast_build_wall_at(mc, village_min_x, z);
        fast_build_wall_at(mc, village_max_x, z);
    }
    
    std::cout << "DEBUG: Wall construction completed!" << std::endl;
}

std::vector<mcpp::Coordinate> find_waypoints(const std::vector<Plot>& plots) {
    std::vector<mcpp::Coordinate> waypoints;
    const Config& config = Config::GetInstance();
    
    if (plots.empty()) {
        const int required_min_waypoints = std::max(1, config.GetVillageSize() / 50 / 5);
        if (required_min_waypoints > 0) {
            std::cerr << "Warning: Could only find 0 waypoints. Required minimum was " << required_min_waypoints << ". Please re-implement the grouping/centroid logic for full compliance." << std::endl;
        }
        return waypoints;
    }

    mcpp::MinecraftConnection mc;

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
        waypoints.emplace_back(center_x, surface_y + 1, center_z);
    } else {
        std::cerr << "Warning: Village centroid is inside a plot and rejected as a waypoint." << std::endl;
    }
    
    const int required_min_waypoints = std::max(1, config.GetVillageSize() / 50 / 5);
    if ((int)waypoints.size() < required_min_waypoints) {
        std::cerr << "Warning: Could only find " << waypoints.size() << " waypoints. Required minimum was " << required_min_waypoints << ". Please re-implement the grouping/centroid logic for full compliance." << std::endl;
    }

    std::cout << "DEBUG: Found " << waypoints.size() << " waypoint(s)" << std::endl;
    return waypoints;
}
