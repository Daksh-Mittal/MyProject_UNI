#include <mcpp/mcpp.h>
#include <vector>
#include <iostream>
#include <ctime>

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

        // --- Defensive check for village location (player position default) ---
        if (Config::GetInstance().GetLocation() == nullptr) {
            // Get player position for default village center
            mcpp::Coordinate player_pos = mc.getPlayerTilePosition();
            // The location will be set using Y=0 as a placeholder, as the actual plot height is determined in find_plots
            Config::GetInstance().SetLocation(new mcpp::Coordinate(player_pos.x, 0, player_pos.z));
            std::cout << "Using player position (" << player_pos.x << ", " << player_pos.z << ") as default village center." << std::endl;
        }
        // -------------------------------------------------------------------

        // OK since a component will never have whitespace (not possible)
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
            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Task A
            std::cout << "Task A: Finding Plots and Terraforming" << std::endl ;

            std::cout << "\t Finding plots..." << std::endl ;

            std::vector<Plot> plots = find_plots() ;

            std::cout << "\t Doing some landscaping..." << std::endl ;

            terraform(plots) ;

            std::cout << "\t Placing a cool wall around the village..." << std::endl ;
            place_wall(plots) ;

            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Task B
            std::cout << "Task B: Building Houses" << std::endl ;
            std::vector<Plot*> pointerisedPlots = pointerisePlotVector(plots);
            buildBuildings(pointerisedPlots);

            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Task C
            std::cout << "Task C: Building Houses" << std::endl ;
            std::vector<Waypoint> waypoints = find_waypoints(plots); // Use found waypoints
            mcpp::Coordinate start(waypoints.front().x, waypoints.front().y, waypoints.front().z);

            std::cout << "\t Connecting waypoints with a fancy path..." << std::endl ;
            connect_waypoints(start, waypoints, mc);

            std::cout << "\t Connecting buildings to waypoints with more fancy paths..." << std::endl ;
            connect_buildings(waypoints, pointerisedPlots, mc);
            
            // Build light pole for waypoints
            for (auto& wp : waypoints) {
                int groundY = mc.getHeight(mcpp::Coordinate2D(wp.x, wp.z));
                auto b = mc.getBlock({wp.x, groundY, wp.z});
                while (b == mcpp::Blocks::GRAVEL || b == mcpp::Blocks::OAK_FENCE || b == mcpp::Blocks::GLOWSTONE) {
                    --groundY;
                    b = mc.getBlock({wp.x, groundY, wp.z});
                }
                for (int h = 0; h < 3; ++h) {
                    mc.setBlock({wp.x, groundY + 1 + h, wp.z}, mcpp::Blocks::OAK_FENCE);
                }
                mc.setBlock({wp.x, groundY + 4, wp.z}, mcpp::Blocks::GLOWSTONE);
            }
        }
    }
    catch(std::exception exception) {
        std::cerr << "Caught exception: " << exception.what() << std::endl;
        returnCode = 1;
    }

    return returnCode;
}
