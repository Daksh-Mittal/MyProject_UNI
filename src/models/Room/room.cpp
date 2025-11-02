#include "room.h"
#include "../PlotRegion/plot_region.h"

Room::Room(PlotRegion region) {
  int y = region.corner1.y; // doesn't really matter which since they should all share the same y level
  int minX = std::min(region.corner1.x, region.corner2.x);
  int minZ = std::min(region.corner1.z, region.corner2.z);
  int maxX = std::max(region.corner1.x, region.corner2.x);
  int maxZ = std::max(region.corner1.z, region.corner2.z);

  cornerTopLeft = mcpp::Coordinate(minX, y, minZ);
  cornerTopRight = mcpp::Coordinate(maxX, y, minZ);
  cornerBottomLeft = mcpp::Coordinate(minX, y, maxZ);
  cornerBottomRight = mcpp::Coordinate(maxX, y, maxZ);
}

RoomRelationship* Room::GetRelationship(Side side) const {
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

mcpp::Coordinate Room::GetCorner(Corner corner) const {
  mcpp::Coordinate point(0,0,0); // necessary to avoid multiple returns

  // we can't use switch here because the style guide prohibits multiple return statements and break, so they aren't viable.
  if (corner == Corner::TopLeft) {
    point = cornerTopLeft;
  }
  else if (corner == Corner::TopRight) {
    point = cornerTopRight;
  }
  else if (corner == Corner::BottomLeft) {
    point = cornerBottomLeft;
  }
  else if (corner == Corner::BottomRight) {
    point = cornerBottomRight;
  }

  return point;
}

mcpp::Coordinate Room::GetInteriorCorner(Corner corner) const {
  mcpp::Coordinate point(0,0,0);

  if (corner == Corner::TopLeft) {
    point = cornerTopLeft + mcpp::Coordinate(1, 1, 1);
  }
  else if (corner == Corner::TopRight) {
    point = cornerTopRight + mcpp::Coordinate(-1, 1, 1);
  }
  else if (corner == Corner::BottomLeft) {
    point = cornerBottomLeft + mcpp::Coordinate(1, 1, -1);
  }
  else if (corner == Corner::BottomRight) {
    point = cornerBottomRight + mcpp::Coordinate(-1, 1, -1);
  }

  return point;
}

mcpp::Coordinate Room::GetCentre(int height) const {
  return mcpp::Coordinate((cornerTopLeft.x + cornerTopRight.x) / 2, cornerTopLeft.y + height, (cornerTopLeft.z + cornerBottomLeft.z) / 2);
}

mcpp::Coordinate Room::GetMidpointOnSide(Side side) const {
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

bool Room::IsMidpointObstructed(Side side) const {
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

bool Room::IsPointOnSide(Side side, mcpp::Coordinate point) const {
  bool isOnSide = false;

  if (side == Side::Top) {
    isOnSide = cornerTopLeft.z == point.z && cornerTopLeft.x <= point.x && cornerTopRight.x >= point.x;
  }
  else if (side == Side::Bottom) {
    isOnSide = cornerBottomLeft.z == point.z && cornerBottomLeft.x <= point.x && cornerBottomRight.x >= point.x;
  }
  else if (side == Side::Left) {
    isOnSide = cornerTopLeft.x == point.x && cornerTopLeft.z <= point.z && cornerBottomLeft.z >= point.z;
  }
  else if (side == Side::Right) {
    isOnSide = cornerTopRight.x == point.x && cornerTopRight.z <= point.z && cornerBottomRight.z >= point.z;
  }

  return isOnSide;
}

std::vector<Side> Room::GetExteriorSides(Plot plot) const {
  std::vector<Side> sides;

  if (cornerTopLeft.x == plot.origin.x && cornerBottomLeft.x == plot.origin.x) sides.push_back(Side::Left);
  else if (cornerTopLeft.z == plot.origin.z && cornerTopRight.z == plot.origin.z) sides.push_back(Side::Top);
  else if (cornerTopRight.x == plot.bound.x && cornerBottomRight.x == plot.bound.x) sides.push_back(Side::Right);
  else if (cornerBottomLeft.z == plot.bound.z && cornerBottomRight.z == plot.bound.z) sides.push_back(Side::Bottom);

  return sides;
}

RoomType Room::GetRoomType() const {
  return roomType;
}

void Room::SetRoomType(RoomType roomType) {
  this->roomType = roomType;
}
