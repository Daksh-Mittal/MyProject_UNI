#include "room_relationship.h"
#include "../Room/room.h"

RoomRelationship::RoomRelationship(Room room1, Room room2) : Room1(&room1), Room2(&room2) {}
