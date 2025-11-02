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
    }
    catch(const std::exception& exception) {
        returnCode = 1;
        std::cerr << exception.what() << std::endl;
    }

    Config::GetInstance().SetMinecraftConnection(&mc);
    std::srand(Config::GetInstance().GetSeed());

    // Defensive check for village location (player position default)
    if (Config::GetInstance().GetLocation() == nullptr) {
        mcpp::Coordinate player_pos = mc.getPlayerTilePosition();
        Config::GetInstance().SetLocation(new mcpp::Coordinate2D(player_pos.x, player_pos.z));
        std::cout << "Using player position (" << player_pos.x << ", " << player_pos.z << ") as default village center." << std::endl;
    }

    if (Config::GetInstance().GetTestedComponentName() != nullptr) {
        std::string component = *Config::GetInstance().GetTestedComponentName();
        int testCase = Config::GetInstance().GetTestCase();
        std::cout << "Testing component '" << component << '\'';
        if (testCase != -1) std::cout << " with test case " << testCase;
        std::cout << std::endl << std::endl;

        if (component == "exterior" || component == "subdivision" || component == "furnishing") {
            std::string errorMessage = test_task_b();

            if (!errorMessage.empty()) {
                returnCode = 1;
                std::cerr << errorMessage << std::endl;
            }
        }
        else if (component == "pathfinding" || component == "connect_buildings") {
            testTaskC(component, testCase, mc);
        }
        else {
            returnCode = 1;
            std::cerr << ("Test parameter '" + component + "' did not match any components. If you did not mean to test a particular component, ensure you are passing --testmode, rather than --testmode=...") << std::endl;
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
        build_buildings(pointerisedPlots);
        
        std::cout << "Task C: Building Paths" << std::endl ;
        std::vector<Waypoint> waypoints = find_waypoints(plots); 
        
        if (!waypoints.empty()) {
            mcpp::Coordinate start(waypoints.front().x, waypoints.front().y, waypoints.front().z);

            std::cout << "\t Connecting waypoints with a fancy path..." << std::endl ;
            connect_waypoints(start, waypoints, mc);

            std::cout << "\t Connecting buildings to waypoints with more fancy paths..." << std::endl ;
            connect_buildings(waypoints, pointerisedPlots, mc);

            std::cout << "\t Building lamps..." << std::endl;
            build_lamp(waypoints, mc);
        
        }
    }

    return returnCode;
}
