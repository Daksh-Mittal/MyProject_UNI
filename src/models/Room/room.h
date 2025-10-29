#ifndef ROOM_H
#define ROOM_H

#include <mcpp/mcpp.h>
#include <vector>
#include "models/PlotRegion/plot_region.h"
#include "models/RoomRelationship/room_relationship.h"
#include "models/Floor/floor.h"
#include "enums/corner.h"
#include "enums/room_type.h"
#include "enums/side.h"

// forward declaration
class Plot;

class Room {
  friend class Floor;

  public:
    void SetRoomType(RoomType roomType);
    RoomType GetRoomType() const;

    RoomRelationship* GetRelationship(Side) const;
    mcpp::Coordinate GetCorner(Corner) const;
    mcpp::Coordinate GetUsableCorner(Corner) const;
    mcpp::Coordinate GetMidpointOnSide(Side) const;
    mcpp::Coordinate GetCentre(int height) const;
    bool IsMidpointObstructed(Side) const;
    bool IsPointOnSide(Side side, mcpp::Coordinate point) const;
    std::vector<Side> GetExteriorSides(Plot plot) const;
    std::vector<Side> GetExteriorSides(Plot plot, bool verbose) const;
    
    Room(PlotRegion region);

  private:
    mcpp::Coordinate cornerTopLeft;
    mcpp::Coordinate cornerTopRight;
    mcpp::Coordinate cornerBottomLeft;
    mcpp::Coordinate cornerBottomRight;

    RoomRelationship* relationAbove = nullptr;
    RoomRelationship* relationLeft = nullptr;
    RoomRelationship* relationRight = nullptr;
    RoomRelationship* relationBelow = nullptr;

    RoomType roomType;
};

#endif
