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

const int MAX_SLOPE_DELTA = 15;
const float MAX_WATER_COVERAGE = 0.15;

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
    int plot_min_x = std::min(origin.x, bound.x);
    int plot_max_x = std::max(origin.x, bound.x);
    int plot_min_z = std::min(origin.z, bound.z);
    int plot_max_z = std::max(origin.z, bound.z);
    int plot_area = (plot_max_x - plot_min_x + 1) * (plot_max_z - plot_min_z + 1);
    int min_y = 256;
    int max_y = 0;
    int water_blocks = 0;

    for (int x = plot_min_x; x <= plot_max_x; ++x) {
        for (int z = plot_min_z; z <= plot_max_z; ++z) {
            int y = get_surface_y(mc, x, z);
            if (y > max_y) max_y = y;
            if (y < min_y) min_y = y;
            mcpp::BlockType block = mc.getBlock(mcpp::Coordinate(x, y + 1, z));
            if (block == mcpp::BlockType(8) || block == mcpp::BlockType(9)) water_blocks++;
        }
    }

    if (max_y - min_y > MAX_SLOPE_DELTA) return false;
    float current_water_coverage = (float)water_blocks / plot_area;
    if (current_water_coverage > MAX_WATER_COVERAGE) return false;
    return true;
}

mcpp::Coordinate determine_entrance(const mcpp::Coordinate& origin, const mcpp::Coordinate& bound) {
    const Config& config = Config::GetInstance();
    int min_x = std::min(origin.x, bound.x);
    int max_x = std::max(origin.x, bound.x);
    int min_z = std::min(origin.z, bound.z);
    int max_z = std::max(origin.z, bound.z);
    int plot_height = origin.y;
    int center_x = config.GetLocationX();
    int center_z = config.GetLocationZ();
    int dist_to_top_edge = std::abs(center_z - max_z);
    int dist_to_bottom_edge = std::abs(center_z - min_z);
    int dist_to_left_edge = std::abs(center_x - min_x);
    int dist_to_right_edge = std::abs(center_x - max_x);
    std::tuple<int, int, int, int, int, char> sides[4] = {
        {dist_to_bottom_edge, 0, min_x, min_z, max_x - min_x, 'X'},
        {dist_to_top_edge, 1, min_x, max_z, max_x - min_x, 'X'},
        {dist_to_left_edge, 2, min_z, min_x, max_z - min_z, 'Z'},
        {dist_to_right_edge, 3, min_z, max_x, max_z - min_z, 'Z'}
    };
    std::sort(sides, sides + 4, [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });
    int start_coord = std::get<2>(sides[0]);
    int fixed_coord = std::get<3>(sides[0]);
    int length = std::get<4>(sides[0]);
    char axis = std::get<5>(sides[0]);
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
    int village_size = config.GetVillageSize();
    int center_x = config.GetLocationX();
    int center_z = config.GetLocationZ();
    bool testmode = config.IsTestMode();
    int village_min_x = center_x - village_size / 2;
    int village_min_z = center_z - village_size / 2;
    int village_max_x = center_x + village_size / 2;
    int village_max_z = center_z + village_size / 2;
    int required_plots = std::max(1, village_size / 50);
    const int MAX_ATTEMPTS = 1000;
    int plot_size_cycle = 14;
    std::srand(config.GetSeed());

    for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
        int current_plot_size;
        int plot_min_x, plot_min_z;
        int plot_max_x, plot_max_z;
        if (testmode) {
            int size_x = village_max_x - village_min_x;
            int size_z = village_max_z - village_min_z;
            int scan_x_index = (attempt * 5) % (size_x + 5);
            int scan_z_index = (attempt * 5) / (size_x / 5 + 1) * 5;
            if (scan_z_index > size_z) {
                if ((int)plots.size() >= required_plots) break;
                scan_z_index = scan_z_index % (size_z + 5);
            }
            plot_min_x = village_min_x + scan_x_index;
            plot_min_z = village_min_z + scan_z_index;
            current_plot_size = plot_size_cycle;
            plot_size_cycle = (plot_size_cycle == 20) ? 14 : plot_size_cycle + 1;
        } else {
            current_plot_size = 14 + (std::rand() % 7);
            int max_rand_x = village_max_x - current_plot_size;
            int max_rand_z = village_max_z - current_plot_size;
            if (max_rand_x < village_min_x || max_rand_z < village_min_z) break;
            plot_min_x = village_min_x + (std::rand() % (max_rand_x - village_min_x + 1));
            plot_min_z = village_min_z + (std::rand() % (max_rand_z - village_min_z + 1));
        }
        plot_max_x = plot_min_x + current_plot_size - 1;
        plot_max_z = plot_min_z + current_plot_size - 1;
        mcpp::Coordinate origin(plot_min_x, 0, plot_min_z);
        mcpp::Coordinate bound(plot_max_x, 0, plot_max_z);
        if (!validate_plot(mc, origin, bound, plots)) continue;
        int plot_height = 0;
        for (int x = plot_min_x; x <= plot_max_x; ++x) {
            for (int z = plot_min_z; z <= plot_max_z; ++z) {
                plot_height = std::max(plot_height, get_surface_y(mc, x, z));
            }
        }
        origin.y = plot_height;
        bound.y = plot_height;
        bool intersects = false;
        Plot proposed_plot(origin, bound, origin, plot_height);
        for (const auto& existing_plot : plots) {
            if (proposed_plot.is_overlapping(existing_plot)) {
                intersects = true;
                break;
            }
        }
        if (intersects) continue;
        mcpp::Coordinate entrance = determine_entrance(origin, bound);
        plots.emplace_back(origin, bound, entrance, plot_height);
        if ((int)plots.size() >= required_plots) {
            if (!testmode) break;
        }
    }

    if ((int)plots.size() < required_plots) {
        std::cerr << "Warning: Could only find " << plots.size() << " plots. Required minimum was " << required_plots << "." << std::endl;
    }
    if (testmode) {
        std::cout << "--- FOUND PLOTS (" << plots.size() << ") ---" << std::endl;
        for (const auto& plot : plots) {
            std::cout << "PLOT_ORIGIN: " << plot.origin.x << "," << plot.origin.y << "," << plot.origin.z
                      << " BOUND: " << plot.bound.x << "," << plot.bound.y << "," << plot.bound.z
                      << " ENTRANCE: " << plot.entrance.x << "," << plot.entrance.y << "," << plot.entrance.z << std::endl;
        }
        std::cout << "--- END PLOTS ---" << std::endl;
    }
    return plots;
}

void terraform(const std::vector<Plot>& plots) {
    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    bool testmode = config.IsTestMode();
    int border_size = config.GetPlotBorder();
    if (testmode) std::cout << "--- TERRAFORMING START ---" << std::endl;
    for (const auto& plot : plots) {
        int plot_h = plot.height;
        int min_x = std::min(plot.origin.x, plot.bound.x) - border_size;
        int max_x = std::max(plot.origin.x, plot.bound.x) + border_size;
        int min_z = std::min(plot.origin.z, plot.bound.z) - border_size;
        int max_z = std::max(plot.origin.z, plot.bound.z) + border_size;
        int plot_min_x = std::min(plot.origin.x, plot.bound.x);
        int plot_max_x = std::max(plot.origin.x, plot.bound.x);
        int plot_min_z = std::min(plot.origin.z, plot.bound.z);
        int plot_max_z = std::max(plot.origin.z, plot.bound.z);
        for (int x = min_x; x <= max_x; ++x) {
            for (int z = min_z; z <= max_z; ++z) {
                int dist_x = 0;
                if (x < plot_min_x) dist_x = plot_min_x - x;
                else if (x > plot_max_x) dist_x = x - plot_max_x;
                int dist_z = 0;
                if (z < plot_min_z) dist_z = plot_min_z - z;
                else if (z > plot_max_z) dist_z = z - plot_max_z;
                int d = std::max(dist_x, dist_z);
                if (d == 0) {
                    for (int y = plot_h; y >= 0; --y) mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::DIRT);
                    mc.setBlock(mcpp::Coordinate(x, plot_h, z), mcpp::Blocks::GRASS);
                    for (int y = plot_h + 1; y < 256; ++y) mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
                } else if (d > 0 && d <= border_size) {
                    int y_g = get_surface_y(mc, x, z);
                    float t = (float)(border_size - d) / border_size;
                    int new_y = (int)std::round(y_g + (plot_h - y_g) * t);
                    mc.setBlock(mcpp::Coordinate(x, new_y, z), mcpp::Blocks::GRASS);
                    for (int y = new_y - 1; y >= 0; --y) mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::DIRT);
                    for (int y = new_y + 1; y < 256; ++y) mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
                }
            }
        }
    }
    if (testmode) std::cout << "--- TERRAFORMING END ---" << std::endl;
}

void place_wall() {
    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    const int WALL_HEIGHT = 4;
    mcpp::BlockType WALL_MATERIAL = mcpp::Blocks::COBBLESTONE;
    int village_size = config.GetVillageSize();
    int center_x = config.GetLocationX();
    int center_z = config.GetLocationZ();
    int village_min_x = center_x - village_size / 2;
    int village_max_x = center_x + village_size / 2;
    int village_min_z = center_z - village_size / 2;
    int village_max_z = center_z + village_size / 2;
    if (config.IsTestMode()) {
        std::cout << "--- VILLAGE WALL START ---" << std::endl;
        std::cout << "WALL_BOUNDS: " << village_min_x << "," << village_min_z << " to " << village_max_x << "," << village_max_z << std::endl;
    }
    auto build_column = [&](int x, int z) {
        int surface_y = get_surface_y(mc, x, z);
        for (int y = surface_y + 1; y < 256; ++y) mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::AIR);
        for (int y = 1; y <= WALL_HEIGHT; ++y) mc.setBlock(mcpp::Coordinate(x, surface_y + y, z), WALL_MATERIAL);
    };
    for (int x = village_min_x; x <= village_max_x; ++x) {
        build_column(x, village_min_z);
        build_column(x, village_max_z);
    }
    for (int z = village_min_z + 1; z < village_max_z; ++z) {
        build_column(village_min_x, z);
        build_column(village_max_x, z);
    }
    if (config.IsTestMode()) std::cout << "--- VILLAGE WALL END ---" << std::endl;
}

std::vector<mcpp::Coordinate> find_waypoints(const std::vector<Plot>& plots) {
    mcpp::MinecraftConnection mc;
    const Config& config = Config::GetInstance();
    std::vector<mcpp::Coordinate> waypoints;
    int village_size = config.GetVillageSize();
    int center_x_config = config.GetLocationX();
    int center_z_config = config.GetLocationZ();
    int required_min_waypoints = (plots.size() / 3) + ((plots.size() % 3 == 0 && plots.size() > 0) ? 0 : 1);
    if (plots.empty()) {
        if (config.IsTestMode()) {
            std::cout << "--- WAYPOINTS (0) ---" << std::endl;
            std::cout << "--- END WAYPOINTS ---" << std::endl;
        }
        return waypoints;
    }
    long long total_x = 0;
    long long total_z = 0;
    for (const auto& plot : plots) {
        total_x += (plot.origin.x + plot.bound.x) / 2;
        total_z += (plot.origin.z + plot.bound.z) / 2;
    }
    int center_x = (plots.empty()) ? center_x_config : (int)(total_x / plots.size());
    int center_z = (plots.empty()) ? center_z_config : (int)(total_z / plots.size());
    auto is_inside_any_plot = [&](int x, int z) {
        for (const auto& plot : plots) {
            if (x >= std::min(plot.origin.x, plot.bound.x) && x <= std::max(plot.origin.x, plot.bound.x) &&
                z >= std::min(plot.origin.z, plot.bound.z) && z <= std::max(plot.origin.z, plot.bound.z)) return true;
        }
        return false;
    };
    if (!is_inside_any_plot(center_x, center_z)) {
        int center_y = get_surface_y(mc, center_x, center_z);
        waypoints.emplace_back(center_x, center_y + 1, center_z);
    }
    std::vector<std::pair<int, int>> potential_offsets = {
        {village_size / 4, village_size / 4},
        {-village_size / 4, village_size / 4},
        {village_size / 4, -village_size / 4},
        {-village_size / 4, -village_size / 4},
    };
    int offset_index = 0;
    while ((int)waypoints.size() < required_min_waypoints && offset_index < (int)potential_offsets.size()) {
        int x = center_x + potential_offsets[offset_index].first;
        int z = center_z + potential_offsets[offset_index].second;
        if (!is_inside_any_plot(x, z)) {
            int y = get_surface_y(mc, x, z);
            waypoints.emplace_back(x, y + 1, z);
        }
        offset_index++;
    }
    if ((int)waypoints.size() < required_min_waypoints) {
        std::cerr << "Warning: Could only find " << waypoints.size() << " waypoints. Required minimum was " << required_min_waypoints << "." << std::endl;
    }
    if (config.IsTestMode()) {
        std::cout << "--- WAYPOINTS (" << waypoints.size() << ") ---" << std::endl;
        for (const auto& wp : waypoints) {
            std::cout << "WAYPOINT: " << wp.x << "," << wp.y << "," << wp.z << std::endl;
        }
        std::cout << "--- END WAYPOINTS ---" << std::endl;
    }
    return waypoints;
}

