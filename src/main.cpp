#include <mcpp/mcpp.h>
#include <vector>
#include <iostream>
#include <ctime>

#include "plots.h"
#include "utils.h"
#include "task_a.h"
#include "task_b.h"
#include "task_c.h"

int main(const int argc, const char *const *const argv) {
    print_student_details() ;

    // MinecraftConnection mc;

    // Read command line arguments:

    int loc_x = 0 ;
    int loc_z = 0 ;
    int village_size = 0 ;
    int plot_border = 0 ;
    int seed = 0 ;
    bool testmode = false ;

    // Demonstrating how to read command line arguments: REMOVE IN FINAL PROGRAM
    std::cout << "There are " << argc << " command line arguments." << std::endl ;
    for (unsigned int i = 0 ; i < argc ; i++) {
        std::cout << argv[i] << std::endl ;
    }

    print_command_line_arguments(loc_x, loc_z, village_size, plot_border, seed, testmode) ;

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Task A
    std::cout << "Task A: Finding Plots and Terraforming" << std::endl ;

    std::cout << "\t Finding plots..." << std::endl ;

    // Insert finding plots here.
    std::vector<Plot> plots = find_plots() ;

    std::cout << "\t Doing some landscaping..." << std::endl ;

    // Insert terraforming here.
    terraform() ;

    // Insert Placing a wall here.
    std::cout << "\t Placing a cool wall around the village..." << std::endl ;
    place_wall() ;

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Task B
    std::cout << "Task B: Building Houses" << std::endl ;
    build_buildings() ;


    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Task C
    std::cout << "Task C: Building Houses" << std::endl ;

    std::cout << "\t Connecting waypoints with a fancy path..." << std::endl ;
    connect_waypoints() ;

    std::cout << "\t Connecting buildings to waypoints with more fancy paths..." << std::endl ;
    connect_buildings() ;



    return 0;
}
