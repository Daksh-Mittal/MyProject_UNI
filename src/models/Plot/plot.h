#ifndef PLOTS_H
#define PLOTS_H

#include <mcpp/mcpp.h>

class Plot {
    public:
        Plot(mcpp::Coordinate origin, mcpp::Coordinate bound);

        // Helper methods
        int get_min_x() const;
        int get_max_x() const;
        int get_min_z() const;
        int get_max_z() const;
        int get_width() const;
        int get_depth() const;
        mcpp::Coordinate get_center() const;
        bool is_overlapping(const Plot& other) const;
        bool intersects_with_border(const Plot& other, int border_size) const;

        mcpp::Coordinate origin;    // minimum/north-west (-x, -z) corner
        mcpp::Coordinate bound;     // maximum/south-east (x, z) corner  
        mcpp::Coordinate entrance;  // for paths and house generation
        int buildingHeight;
};

#endif
