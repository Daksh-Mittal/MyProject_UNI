#ifndef ROOM_RELATIONSHIP_H
#define ROOM_RELATIONSHIP_H

// forward declaration to avoid circular dependency with Room
class Room;

struct RoomRelationship {
  Room *Room1;
  Room *Room2;
  mcpp::Coordinate ConnectionPoint;
  bool DoorBuilt = false;

  RoomRelationship(Room, Room);
};

#endif
