#include "plot.h"
#include <algorithm>

Plot::Plot(mcpp::Coordinate origin, mcpp::Coordinate bound) : origin(origin), bound(bound) {}
Plot::Plot(mcpp::Coordinate origin, mcpp::Coordinate bound, mcpp::Coordinate entrance) : origin(origin), bound(bound), entrance(entrance), useDefaultEntrance(false) {}

int Plot::get_min_x() const {
    return std::min(origin.x, bound.x);
}

int Plot::get_max_x() const {
    return std::max(origin.x, bound.x);
}

int Plot::get_min_z() const {
    return std::min(origin.z, bound.z);
}

int Plot::get_max_z() const {
    return std::max(origin.z, bound.z);
}

int Plot::get_width() const {
    return get_max_x() - get_min_x() + 1;
}

int Plot::get_depth() const {
    return get_max_z() - get_min_z() + 1;
}

mcpp::Coordinate Plot::get_center() const {
    return mcpp::Coordinate(
        (get_min_x() + get_max_x()) / 2,
        origin.y,
        (get_min_z() + get_max_z()) / 2
    );
}

bool Plot::is_overlapping(const Plot& other) const {
    // Plots must not intersect each other
    bool overlap_x = (get_max_x() >= other.get_min_x()) && (other.get_max_x() >= get_min_x());
    bool overlap_z = (get_max_z() >= other.get_min_z()) && (other.get_max_z() >= get_min_z());
    
    return overlap_x && overlap_z;
}

bool Plot::intersects_with_border(const Plot& other, int border_size) const {
    // Check if plot borders intersect (plots themselves can be closer than borders)
    int this_min_x_with_border = get_min_x() - border_size;
    int this_max_x_with_border = get_max_x() + border_size;
    int this_min_z_with_border = get_min_z() - border_size;
    int this_max_z_with_border = get_max_z() + border_size;
    
    int other_min_x_with_border = other.get_min_x() - border_size;
    int other_max_x_with_border = other.get_max_x() + border_size;
    int other_min_z_with_border = other.get_min_z() - border_size;
    int other_max_z_with_border = other.get_max_z() + border_size;
    
    bool overlap_x = (this_max_x_with_border >= other_min_x_with_border) && 
                    (other_max_x_with_border >= this_min_x_with_border);
    bool overlap_z = (this_max_z_with_border >= other_min_z_with_border) && 
                    (other_max_z_with_border >= this_min_z_with_border);
    
    return overlap_x && overlap_z;
}
