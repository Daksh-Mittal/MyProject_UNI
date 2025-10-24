#ifndef WAYPOINT_H
#define WAYPOINT_H
#include <vector>

struct Waypoint {
    int x, y, z;
    bool hasParent = false;

    bool operator<(const Waypoint& o) const {
        if (x != o.x) return x < o.x;
        if (y != o.y) return y < o.y;
        return z < o.z;
    }
};

std::vector<Waypoint> get_waypoints();
#endif
