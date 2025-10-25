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

        // OK since a component will never have whitespace (not possible)
        if (Config::GetInstance().GetTestedComponentName() != "NOT SPECIFIED" && Config::GetInstance().IsTestMode()) {
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

            // Insert finding plots here.
            std::vector<Plot> plots = find_plots() ;

            std::cout << "\t Doing some landscaping..." << std::endl ;

            // Insert terraforming here.
            terraform(plots) ;

            // Insert Placing a wall here.
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
            std::vector<Waypoint> waypoints = get_waypoints();
            mcpp::Coordinate start(waypoints[0].x, waypoints[0].y, waypoints[0].z);

            std::cout << "\t Connecting waypoints with a fancy path..." << std::endl ;
            connect_waypoints(start, waypoints, mc);

            std::cout << "\t Connecting buildings to waypoints with more fancy paths..." << std::endl ;
            connect_buildings(waypoints, pointerisedPlots, mc);
            //builds light pole
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
        std::cerr << exception.what() << std::endl;
        returnCode = 1;
    }

    return returnCode;
}
