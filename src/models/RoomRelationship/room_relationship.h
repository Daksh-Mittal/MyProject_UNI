#ifndef ROOM_RELATIONSHIP_H
#define ROOM_RELATIONSHIP_H

#include <mcpp/mcpp.h>
#include "enums/side.h"

// forward declaration to avoid circular dependency with Room
class Room;

struct RoomRelationship {
  Room* Room1;
  Room* Room2;
  mcpp::Coordinate ConnectionPoint;
  Side Orientation;
  bool DoorBuilt = false;

  RoomRelationship(Room*, Room*);
};

#endif

