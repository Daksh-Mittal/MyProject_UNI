#ifndef PLOT_REGION_H
#define PLOT_REGION_H

#include <mcpp/mcpp.h>
#include "../Plot/plot.h"
#include "../../enums/axis.h"

// need a forward declaration here to avoid a circular dependency
class Room;

// Represents a subdividable region within a plot
class PlotRegion {
  friend class Room;

  public:
    Axis GetSubdivisionAxis(bool isTestMode);
    PlotRegion Subdivide(Axis axis, bool isTestMode);
    int GetLength();
    int GetWidth();

    PlotRegion(mcpp::Coordinate corner1, mcpp::Coordinate corner2);
    PlotRegion(Plot plot);

  private:
    static const int MIN_SIZE = 5; // 4 + 1 (we are deciding where the wall will go, so it can go at or after the 6th block)
    static const int MAX_SIZE = 10; // 8 + 2 (this accounts for the walls taking up 1 block each, otherwise smaller rooms become really small)

    mcpp::Coordinate corner1;
    mcpp::Coordinate corner2;
};

#endif
