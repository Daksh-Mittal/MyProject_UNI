#ifndef ROOM_H
#define ROOM_H

#include <mcpp/mcpp.h>
#include <vector>
#include "../PlotRegion/plot_region.h"
#include "../RoomRelationship/room_relationship.h"
#include "../Floor/floor.h"
#include "../../enums/corner.h"
#include "../../enums/room_type.h"
#include "../../enums/side.h"

class Room {
  friend class Floor;

  public:
    void SetRoomType(RoomType roomType);

    RoomRelationship* GetRelationship(Side);
    mcpp::Coordinate GetCorner(Corner);
    mcpp::Coordinate GetMidpointOnSide(Side);
    bool IsMidpointObstructed(Side);
    bool IsPointOnSide(Side side, mcpp::Coordinate point);
    Room(PlotRegion region);

  private:
    mcpp::Coordinate cornerTopLeft;
    mcpp::Coordinate cornerTopRight;
    mcpp::Coordinate cornerBottomLeft;
    mcpp::Coordinate cornerBottomRight;

    RoomRelationship* relationAbove;
    RoomRelationship* relationLeft;
    RoomRelationship* relationRight;
    RoomRelationship* relationBelow;

    RoomType roomType;
};

#endif
