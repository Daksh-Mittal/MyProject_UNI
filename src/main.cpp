#include <mcpp/mcpp.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <exception>

#include "models/Plot/plot.h"
#include "utils.h"
#include "task_a.h"
#include "task_b.h"
#include "task_c.h"
#include "config.h"

int main(const int argc, const char *argv[]) {
    int returnCode = 0;
    mcpp::MinecraftConnection mc;
    printStudentDetails();

    try {
        Config::GetInstance().ApplyConfiguration(argc, argv);
        Config::GetInstance().SetMinecraftConnection(&mc);
        std::srand(Config::GetInstance().GetSeed());

        // Defensive check for village location (player position default)
        if (Config::GetInstance().GetLocation() == nullptr) {
            mcpp::Coordinate player_pos = mc.getPlayerTilePosition();
            Config::GetInstance().SetLocation(new mcpp::Coordinate(player_pos.x, 0, player_pos.z));
            std::cout << "Using player position (" << player_pos.x << ", " << player_pos.z << ") as default village center." << std::endl;
        }

        if (Config::GetInstance().GetTestedComponentName() != "NOT SET" && Config::GetInstance().IsTestMode()) {
            std::string component = Config::GetInstance().GetTestedComponentName();
            int testCase = Config::GetInstance().GetTestCase();
            std::cout << "Testing component '" << component << '\'';
            if (testCase != -1) std::cout << " with test case " << testCase;
            std::cout << std::endl << std::endl;

            if (component == "exterior" || component == "subdivision" || component == "furnishing") {
                testTaskB();
            }
            else if (component == "pathfinding" || component == "connect_buildings") {
                testTaskC(component, testCase, mc);
            }

        }
        else {
            std::cout << "Task A: Finding Plots and Terraforming" << std::endl ;

            std::cout << "\t Finding plots..." << std::endl ;

            std::vector<Plot> plots = find_plots() ;

            std::cout << "\t Doing some landscaping..." << std::endl ;

            terraform(plots) ;

            std::cout << "\t Placing a cool wall around the village..." << std::endl ;
            place_wall(plots) ;

            std::cout << "Task B: Building Houses" << std::endl ;
            std::vector<Plot*> pointerisedPlots = pointerisePlotVector(plots);
            buildBuildings(pointerisedPlots);

            std::cout << "Task C: Building Houses" << std::endl ;
            std::vector<Waypoint> waypoints = find_waypoints(plots); 
            
            if (!waypoints.empty()) {
                mcpp::Coordinate start(waypoints.front().x, waypoints.front().y, waypoints.front().z);

                std::cout << "\t Connecting waypoints with a fancy path..." << std::endl ;
                connect_waypoints(start, waypoints, mc);

                std::cout << "\t Connecting buildings to waypoints with more fancy paths..." << std::endl ;
                connect_buildings(waypoints, pointerisedPlots, mc);
                
                // Build light pole for waypoints
                for (auto& wp : waypoints) {
                    // FIX 1: Pass X and Z as separate ints to mc.getHeight
                    int groundY = mc.getHeight(wp.x, wp.z);
                    
                    // FIX 2: Explicitly call mcpp::Coordinate constructor
                    auto b = mc.getBlock(mcpp::Coordinate(wp.x, groundY, wp.z));
                    
                    while (b == mcpp::Blocks::GRAVEL || b == mcpp::Blocks::OAK_FENCE || b == mcpp::Blocks::GLOWSTONE) {
                        --groundY;
                        // FIX 2: Explicitly call mcpp::Coordinate constructor
                        b = mc.getBlock(mcpp::Coordinate(wp.x, groundY, wp.z));
                    }
                    for (int h = 0; h < 3; ++h) {
                        // FIX 2: Explicitly call mcpp::Coordinate constructor
                        mc.setBlock(mcpp::Coordinate(wp.x, groundY + 1 + h, wp.z), mcpp::Blocks::OAK_FENCE);
                    }
                    // FIX 2: Explicitly call mcpp::Coordinate constructor
                    mc.setBlock(mcpp::Coordinate(wp.x, groundY + 4, wp.z), mcpp::Blocks::GLOWSTONE);
                }
            } else {
                 std::cerr << "\t Warning: No waypoints found to build paths/structures." << std::endl;
            }
        }
    }
    // FIX 3: Catch by constant reference
    catch(const std::exception& exception) {
        std::cerr << "Caught exception: " << exception.what() << std::endl;
        returnCode = 1;
    }

    return returnCode;
}
