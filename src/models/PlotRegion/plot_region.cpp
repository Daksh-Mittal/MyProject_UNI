#include "plot_region.h"

PlotRegion::PlotRegion(mcpp::Coordinate corner1, mcpp::Coordinate corner2) : corner1(corner1), corner2(corner2) {}
PlotRegion::PlotRegion(Plot plot) : corner1(plot.origin), corner2(plot.bound) {}

int PlotRegion::GetLength() const {
  return std::abs(corner1.x - corner2.x);
}

int PlotRegion::GetWidth() const {
  return std::abs(corner1.z - corner2.z);
}

Axis PlotRegion::GetSubdivisionAxis() {
  Axis axis = Axis::None;

  if (GetLength() >= MAX_SIZE && GetWidth() >= MAX_SIZE) {
    if (Config::GetInstance().IsTestMode()) {
      // take the longer of the two sides if both are suitable candidates. this is required by specification but doesn't ultimately matter?
      axis = (GetLength() >= GetWidth()) ? Axis::X : Axis::Z;
    }
    else {
      axis = (std::rand() % 2 == 0) ? Axis::X : Axis::Z;
    }
  }  
  else if (GetLength() >= MAX_SIZE) {
    axis = Axis::X;
  }
  else if (GetWidth() >= MAX_SIZE) {
    axis = Axis::Z;
  }

  return axis;
}

// Mutates this PlotRegion and creates a new one
// (Minimises unnecessary memory usage)
PlotRegion PlotRegion::Subdivide(Axis axis, const Plot& plot) {
  bool couldRandomise = false;
  //PlotRegion *regionPtr = nullptr; // TODO: style guide prohibits multiple return statements but we will use it for now to avoid errors

  if (axis == Axis::X) {
    int point = std::min(corner1.x, corner2.x) + MIN_SIZE;

    if (Config::GetInstance().IsTestMode()) {
      point = (corner1.x + corner2.x) / 2; // midpoint in test mode
    }
    else {
      int dist = corner2.x - corner1.x;
      int acceptableDist = dist - 2*MIN_SIZE; // reserve 5 blocks for each side

      if (acceptableDist > 1) {
        int offset = MIN_SIZE + std::rand() % acceptableDist;
        couldRandomise = true;
        point = corner1.x + offset;
      }
    }

    mcpp::Coordinate topLeftCorner(point, corner1.y, corner1.z);
    mcpp::Coordinate bottomRightCorner(point, corner2.y, corner2.z);

    // if the default entrance is being used, we don't need to account for whether or not a subdivision would block the entrance
    if (!plot.useDefaultEntrance) {
      if (topLeftCorner == plot.entrance || bottomRightCorner == plot.entrance) {
        throw subdivision_error("Subdivision would block entrance", couldRandomise);
      }
    }

    PlotRegion region(topLeftCorner, corner2);
    corner2 = bottomRightCorner; // new bottom right corner is midpoint
    return region;
  }
  else if (axis == Axis::Z) {
    int point = corner1.z + MIN_SIZE;

    if (Config::GetInstance().IsTestMode()) {
      point = (corner1.z + corner2.z) / 2; // midpoint in test mode
    }
    else {
      int dist = corner2.z - corner1.z;
      int acceptableDist = dist - 2*MIN_SIZE; // reserve 5 blocks for each side

      if (acceptableDist > 1) {
        int offset = MIN_SIZE + std::rand() % acceptableDist;
        point = corner1.z + offset;
      }
    }

    mcpp::Coordinate bottomRightCorner(corner2.x, corner2.y, point);
    mcpp::Coordinate topLeftCorner(corner1.x, corner1.y, point);

    if (!plot.useDefaultEntrance) {
      if (topLeftCorner == plot.entrance || bottomRightCorner == plot.entrance) {
        throw subdivision_error("Subdivision would block entrance", couldRandomise);
      }
    }

    PlotRegion region(topLeftCorner, corner2);
    corner2 = bottomRightCorner; // new bottom right corner is midpoint
    return region;
  }

  throw std::invalid_argument("NEVER");
  //return *regionPtr;
}

mcpp::Coordinate PlotRegion::GetTopLeftCorner() const {
  return corner1;
}

mcpp::Coordinate PlotRegion::GetBottomRightCorner() const {
  return corner2;
}

