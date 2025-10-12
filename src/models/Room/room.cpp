#include "room.h"
#include "../PlotRegion/plot_region.h"

Room::Room(PlotRegion region) {
  int y = region.corner1.y; // doesn't really matter which since they should all share the same y level
  int minX = std::min(region.corner1.x, region.corner2.x);
  int minZ = std::min(region.corner1.z, region.corner2.z);
  int maxX = std::max(region.corner1.x, region.corner2.x);
  int maxZ = std::max(region.corner1.z, region.corner2.z);

  cornerTopLeft = mcpp::Coordinate(minX, y, maxZ);
  cornerTopRight = mcpp::Coordinate(maxX, y, maxZ);
  cornerBottomLeft = mcpp::Coordinate(minX, y, minZ);
  cornerBottomRight = mcpp::Coordinate(maxX, y, minZ);
}

RoomRelationship* Room::GetRelationship(Side side) {
  RoomRelationship* relationship = nullptr;

  if (side == Side::Top) {
    relationship = relationAbove;
  }
  else if (side == Side::Left) {
    relationship = relationLeft;
  }
  else if (side == Side::Right) {
    relationship = relationRight;
  }
  else if (side == Side::Bottom) {
    relationship = relationBelow;
  }

  return relationship;
}

mcpp::Coordinate Room::GetCorner(Corner corner) {
  mcpp::Coordinate *point = nullptr; // necessary to avoid multiple returns

  // we can't use switch here because the style guide prohibits multiple return statements and break, so they aren't viable.
  if (corner == Corner::TopLeft) {
    point = &cornerTopLeft;
  }
  else if (corner == Corner::TopRight) {
    point = &cornerTopRight;
  }
  else if (corner == Corner::BottomLeft) {
    point = &cornerBottomLeft;
  }
  else if (corner == Corner::BottomRight) {
    point = &cornerBottomRight;
  }

  return *point;
}

mcpp::Coordinate Room::GetMidpointOnSide(Side side) {
  mcpp::Coordinate midpoint;

  if (side == Side::Top) {
    midpoint = mcpp::Coordinate((cornerTopLeft.x + cornerTopRight.x) / 2, cornerTopLeft.y, cornerTopLeft.z);
  }
  else if (side == Side::Bottom) {
    midpoint = mcpp::Coordinate((cornerBottomLeft.x + cornerBottomRight.x) / 2, cornerBottomLeft.y, cornerBottomLeft.z);
  }
  else if (side == Side::Left) {
    midpoint = mcpp::Coordinate(cornerTopLeft.x, cornerBottomLeft.y, (cornerTopLeft.z + cornerBottomLeft.z) / 2);
  }
  else if (side == Side::Right) {
    midpoint = mcpp::Coordinate(cornerTopRight.x, cornerBottomRight.y, (cornerTopRight.z + cornerBottomRight.z) / 2);
  }

  return midpoint;
}

bool Room::IsMidpointObstructed(Side side) {
  mcpp::MinecraftConnection mc;
  mcpp::Coordinate midpoint = GetMidpointOnSide(side);
  bool isObstructed = false;

  if (side == Side::Top) {
    isObstructed = mc.getBlock(mcpp::Coordinate(midpoint.x, midpoint.y, midpoint.z - 1)) != mcpp::Blocks::AIR;
  }
  else if (side == Side::Bottom) {
    isObstructed = mc.getBlock(mcpp::Coordinate(midpoint.x, midpoint.y, midpoint.z + 1)) != mcpp::Blocks::AIR;
  }
  else if (side == Side::Left) {
    isObstructed = mc.getBlock(mcpp::Coordinate(midpoint.x - 1, midpoint.y, midpoint.z)) != mcpp::Blocks::AIR;
  }
  else if (side == Side::Right) {
    isObstructed = mc.getBlock(mcpp::Coordinate(midpoint.x + 1, midpoint.y, midpoint.z)) != mcpp::Blocks::AIR;
  }

  return isObstructed;
}

bool Room::IsPointOnSide(Side side, mcpp::Coordinate point) {
  bool isOnSide = false;

  if (side == Side::Top) {
    isOnSide = cornerTopLeft.z == point.z && cornerTopLeft.x < point.x && cornerTopRight.x > point.x;
  }
  else if (side == Side::Bottom) {
    isOnSide = cornerBottomLeft.z == point.z && cornerBottomLeft.x < point.x && cornerBottomRight.x > point.x;
  }
  else if (side == Side::Left) {
    isOnSide = cornerTopLeft.x == point.x && cornerTopLeft.z > point.z && cornerBottomLeft.x < point.z;
  }
  else if (side == Side::Right) {
    isOnSide = cornerTopRight.x == point.x && cornerTopRight.z > point.z && cornerBottomRight.x < point.z;
  }

  return isOnSide;
}
