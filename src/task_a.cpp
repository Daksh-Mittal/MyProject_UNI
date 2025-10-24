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

// Function to find the highest non-air/non-water block at (x, z)
int get_surface_y(mcpp::MinecraftConnection& mc, int x, int z) {
    for (int y = 255; y >= 0; --y) {
        mcpp::Coordinate current_coord(x, y, z);
        mcpp::BlockType block = mc.getBlock(current_coord);
        // Blocks::AIR (0), BlockType(8) for Flowing Water, BlockType(9) for Still Water
        if (block != mcpp::Blocks::AIR && block != mcpp::BlockType(8) && block != mcpp::BlockType(9)) {
            return y;
        }
    }
    return 63; // Default Minecraft surface level
}

// Validates plot based on terrain and water coverage
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
            
            // Check for water *above* the surface block
            mcpp::BlockType block = mc.getBlock(mcpp::Coordinate(x, y + 1, z));
            if (block == mcpp::BlockType(8) || block == mcpp::BlockType(9)) water_blocks++;
        }
    }

    // A maximum slope delta of 15
    int slope_delta = max_y - min_y;
    if (slope_delta > 15) {
        std::cout << "DEBUG: REJECTED - Slope too steep: " << slope_delta << std::endl;
        return false;
    }
    
    // A surface water coverage of <= 15%
    float current_water_coverage = (float)water_blocks / plot_area;
    if (current_water_coverage > 0.15) {
        std::cout << "DEBUG: REJECTED - Too much water: " << current_water_coverage << std::endl;
        return false;
    }
    
    // Check border intersection with village boundary
    int village_size = config.GetVillageSize();
    mcpp::Coordinate2D* location = config.GetLocation();
    int center_x = location->x;
    int center_z = location->z;
    
    int village_min_x = center_x - village_size / 2;
    int village_max_x = center_x + village_size / 2;
    int village_min_z = center_z - village_size / 2;
    int village_max_z = center_z + village_size / 2;
    
    int border_min_x = plot_min_x - plot_border;
    int border_max_x = plot_max_x + plot_border;
    int border_min_z = plot_min_z - plot_border;
    int border_max_z = plot_max_z + plot_border;
    
    if (border_min_x < village_min_x || border_max_x > village_max_x ||
        border_min_z < village_min_z || border_max_z > village_max_z) {
        std::cout << "DEBUG: REJECTED - Border exceeds village boundary" << std::endl;
        return false;
    }
    
    std::cout << "DEBUG: ACCEPTED - Plot is valid" << std::endl;
    return true;
}

// Determines the plot entrance facing the village center
mcpp::Coordinate determine_entrance(const mcpp::Coordinate& origin, const mcpp::Coordinate& bound) {
    const Config& config = Config::GetInstance();
    int min_x = std::min(origin.x, bound.x);
    int max_x = std::max(origin.x, bound.x);
    int min_z = std::min(origin.z, bound.z);
    int max_z = std::max(origin.z, bound.z);
    int plot_height = origin.y;
    mcpp::Coordinate2D* location = config.GetLocation();
    int center_x = location->x;
    int center_z = location->z;

    // Calculate distance of each edge to the village center
    int dist_to_bottom_edge = std::abs(center_z - min_z);
    int dist_to_top_edge = std::abs(center_z - max_z);
    int dist_to_left_edge = std::abs(center_x - min_x);
    int dist_to_right_edge = std::abs(center_x - max_x);

    // Find the edge closest to the center
    int min_dist = std::min({dist_to_bottom_edge, dist_to_top_edge, dist_to_left_edge, dist_to_right_edge});
    
    mcpp::Coordinate entrance(0, plot_height + 1, 0);
    
    if (min_dist == dist_to_bottom_edge) {
        // Bottom edge (min_z)
        entrance.x = (min_x + max_x) / 2;
        entrance.z = min_z;
    } else if (min_dist == dist_to_top_edge) {
        // Top edge (max_z)
        entrance.x = (min_x + max_x) / 2;
        entrance.z = max_z;
    } else if (min_dist == dist_to_left_edge) {
        // Left edge (min_x)
        entrance.x = min_x;
        entrance.z = (min_z + max_z) / 2;
    } else {
        // Right edge (max_x)
        entrance.x = max_x;
        entrance.z = (min_z + max_z) / 2;
    }
    
    return entrance;
}

// Finds suitable plots within the village area
std::vector<Plot> find_plots() {
    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    std::vector<Plot> plots;
    int village_size = config.GetVillageSize();
    mcpp::Coordinate2D* location = config.GetLocation();
    int center_x = location->x;
    int center_z = location->z;
    bool testmode = config.IsTestMode();
   // int plot_border = config.GetPlotBorder();

    std::cout << "DEBUG: Finding plots around (" << center_x << "," << center_z 
              << ") with size " << village_size << std::endl;

    // Village area bounds
    int village_min_x = center_x - village_size / 2;
    int village_min_z = center_z - village_size / 2;
    int village_max_x = center_x + village_size / 2;
    int village_max_z = center_z + village_size / 2;

    std::cout << "DEBUG: Village bounds: X[" << village_min_x << "-" << village_max_x 
              << "] Z[" << village_min_z << "-" << village_max_z << "]" << std::endl;

    int required_plots = std::max(1, village_size / 50);
    const int MAX_ATTEMPTS = 1000;
    const int MIN_PLOT_SIZE = 14;
    const int MAX_PLOT_SIZE = 20;
    
    std::srand(config.GetSeed());

    int attempts = 0;
    int plot_size_cycle = MIN_PLOT_SIZE;

    while (attempts < MAX_ATTEMPTS && (int)plots.size() < required_plots) {
        int current_plot_size;
        int plot_min_x, plot_min_z;

        if (testmode) {
            // Test mode: scan grid in 5 block increments
            int grid_size = 5;
            int grid_cells = (village_size / grid_size) + 1;
            int grid_index = attempts % (grid_cells * grid_cells);
            
            int grid_x = (grid_index % grid_cells) * grid_size;
            int grid_z = (grid_index / grid_cells) * grid_size;
            
            plot_min_x = village_min_x + grid_x;
            plot_min_z = village_min_z + grid_z;
            
            current_plot_size = plot_size_cycle;
            plot_size_cycle = (plot_size_cycle == MAX_PLOT_SIZE) ? MIN_PLOT_SIZE : plot_size_cycle + 1;
        } else {
            // Random mode
            current_plot_size = MIN_PLOT_SIZE + (std::rand() % (MAX_PLOT_SIZE - MIN_PLOT_SIZE + 1));
            plot_min_x = village_min_x + (std::rand() % (village_size - current_plot_size));
            plot_min_z = village_min_z + (std::rand() % (village_size - current_plot_size));
        }

        int plot_max_x = plot_min_x + current_plot_size - 1;
        int plot_max_z = plot_min_z + current_plot_size - 1;

        // Check bounds
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

        // Determine plot height (maximum surface height within plot)
        int plot_height = 0;
        for (int x = plot_min_x; x <= plot_max_x; ++x) {
            for (int z = plot_min_z; z <= plot_max_z; ++z) {
                plot_height = std::max(plot_height, get_surface_y(mc, x, z));
            }
        }

        origin.y = plot_height;
        bound.y = plot_height;

        // Check for intersection with existing plots
        Plot proposed_plot(origin, bound, origin, plot_height, current_plot_size);
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

        // Determine entrance
        mcpp::Coordinate entrance = determine_entrance(origin, bound);

        plots.emplace_back(origin, bound, entrance, plot_height, current_plot_size);
        std::cout << "DEBUG: Added plot #" << plots.size() << " at (" << plot_min_x << "," << plot_min_z 
                  << ") size " << current_plot_size << " height " << plot_height << std::endl;

        attempts++;
    }

    if ((int)plots.size() < required_plots) {
        std::cerr << "Warning: Could only find " << plots.size() << " plots. Required minimum was " << required_plots << "." << std::endl;
    }

    std::cout << "DEBUG: Found " << plots.size() << " plots (required: " << required_plots << ")" << std::endl;
    return plots;
}

// Terraforms the area around the plots using linear interpolation
void terraform(const std::vector<Plot>& plots) {
    if (plots.empty()) {
        std::cout << "DEBUG: No plots to terraform" << std::endl;
        return;
    }

    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    int border_size = config.GetPlotBorder();
    
    std::cout << "DEBUG: Starting terraforming for " << plots.size() << " plots" << std::endl;

    for (const auto& plot : plots) {
        int plot_height = plot.origin.y;
        int plot_min_x = std::min(plot.origin.x, plot.bound.x);
        int plot_max_x = std::max(plot.origin.x, plot.bound.x);
        int plot_min_z = std::min(plot.origin.z, plot.bound.z);
        int plot_max_z = std::max(plot.origin.z, plot.bound.z);
        
        std::cout << "DEBUG: Terraforming plot at (" << plot_min_x << "," << plot_min_z 
                  << ") height " << plot_height << std::endl;

        // Area to terraform (plot + border)
        int terra_min_x = plot_min_x - border_size;
        int terra_max_x = plot_max_x + border_size;
        int terra_min_z = plot_min_z - border_size;
        int terra_max_z = plot_max_z + border_size;

        for (int x = terra_min_x; x <= terra_max_x; ++x) {
            for (int z = terra_min_z; z <= terra_max_z; ++z) {
                
                // Calculate distance from plot edge
                int dist_x = 0;
                if (x < plot_min_x) dist_x = plot_min_x - x;
                else if (x > plot_max_x) dist_x = x - plot_max_x;
                
                int dist_z = 0;
                if (z < plot_min_z) dist_z = plot_min_z - z;
                else if (z > plot_max_z) dist_z = z - plot_max_z;
                
                int distance = std::max(dist_x, dist_z);
                
                if (distance == 0) {
                    // Inside plot: create flat surface at plot height
                    // Clear above plot height
                    for (int y = plot_height + 1; y < 256; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
                    }
                    // Fill below plot height with dirt, top with grass
                    for (int y = 0; y <= plot_height; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::DIRT);
                    }
                    mc.setBlock(mcpp::Coordinate(x, plot_height, z), mcpp::Blocks::GRASS);
                    
                } else if (distance <= border_size) {
                    // In border area: apply linear smoothing
                    int ground_height = get_surface_y(mc, x, z);
                    
                    // Linear interpolation: y = y_g + (y_p - y_g) * (p - d) / p
                    double blend_factor = static_cast<double>(border_size - distance) / border_size;
                    int new_height = static_cast<int>(std::round(
                        ground_height + (plot_height - ground_height) * blend_factor
                    ));
                    
                    // Ensure new height is reasonable
                    new_height = std::max(0, std::min(255, new_height));
                    
                    // Clear above new height
                    for (int y = new_height + 1; y < 256; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
                    }
                    // Fill to new height
                    for (int y = 0; y <= new_height; ++y) {
                        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::DIRT);
                    }
                    mc.setBlock(mcpp::Coordinate(x, new_height, z), mcpp::Blocks::GRASS);
                }
            }
        }
    }
    std::cout << "DEBUG: Terraforming completed" << std::endl;
}

// Builds the village wall around the boundary
void place_wall(const std::vector<Plot>& plots) {
    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    int village_size = config.GetVillageSize();
    mcpp::Coordinate2D* location = config.GetLocation();
    int center_x = location->x;
    int center_z = location->z;
    
    std::cout << "DEBUG: Building wall around (" << center_x << "," << center_z 
              << ") size " << village_size << std::endl;

    int village_min_x = center_x - village_size / 2;
    int village_max_x = center_x + village_size / 2;
    int village_min_z = center_z - village_size / 2;
    int village_max_z = center_z + village_size / 2;

    // Use fast wall building for speed
    std::cout << "Building walls (fast method)..." << std::endl;

    // Build walls in batches
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

// Fast wall building - minimal block operations
void fast_build_wall_at(mcpp::MinecraftConnection& mc, int x, int z) {
    int surface_y = get_surface_y(mc, x, z);
    
    // Build 3-block high wall (faster than 4)
    mc.setBlock(mcpp::Coordinate(x, surface_y + 1, z), mcpp::Blocks::COBBLESTONE);
    mc.setBlock(mcpp::Coordinate(x, surface_y + 2, z), mcpp::Blocks::COBBLESTONE);
    mc.setBlock(mcpp::Coordinate(x, surface_y + 3, z), mcpp::Blocks::COBBLESTONE);
}

void build_wall_at(mcpp::MinecraftConnection& mc, int x, int z) {
    int surface_y = get_surface_y(mc, x, z);
    
    // Clear space for wall
    for (int y = surface_y + 1; y <= surface_y + 5; ++y) {
        mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
    }
    
    // Build 4-block high wall
    for (int y = 1; y <= 4; ++y) {
        mc.setBlock(mcpp::Coordinate(x, surface_y + y, z), mcpp::Blocks::COBBLESTONE);
    }
}

// Find waypoints using grouping algorithm
std::vector<mcpp::Coordinate> find_waypoints(const std::vector<Plot>& plots) {
    std::vector<mcpp::Coordinate> waypoints;
    
    if (plots.empty()) {
        return waypoints;
    }

    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();

    mcpp::Coordinate2D* location = config.GetLocation();
    int center_x = location->x;
    int center_z = location->z;
    int surface_y = get_surface_y(mc, center_x, center_z);
    
    waypoints.emplace_back(center_x, surface_y + 1, center_z);
    
    std::cout << "DEBUG: Found " << waypoints.size() << " waypoints" << std::endl;
    return waypoints;
}
