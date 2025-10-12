#include "floor.h"
#include "../RoomRelationship/room_relationship.h"
#include "../Room/room.h"

Floor::Floor(std::vector<Room>& rooms) {
  this->rooms = &rooms;
}

// we must clean up RoomRelationships since they are loaded into memory by 'new'
Floor::~Floor() {
  for (RoomRelationship*& relation : relations) {
    delete relation;
  }
}

void Floor::AssignRelationships() {
  for (size_t i = 0; i < rooms->size(); ++i) {
    for (size_t j = i + 1; j < rooms->size(); ++j) {
      // Compare rooms[i] with rooms[j] - ensures each pair is visited once, and no room is compared with itself
      Room room1 = (*rooms)[i];
      Room room2 = (*rooms)[j];
      RoomRelationship *relation = nullptr;

      if (room1.relationBelow == nullptr && room2.relationAbove == nullptr && room1.IsPointOnSide(Side::Bottom, room2.GetMidpointOnSide(Side::Top))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2.GetMidpointOnSide(Side::Top);
        room1.relationBelow = relation;
        room2.relationAbove = relation;
      }
      else if (room1.relationLeft == nullptr && room2.relationRight == nullptr && room1.IsPointOnSide(Side::Left, room2.GetMidpointOnSide(Side::Right))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2.GetMidpointOnSide(Side::Right);
        room1.relationLeft = relation;
        room2.relationRight = relation;
      }
      else if (room1.relationRight == nullptr && room2.relationLeft == nullptr && room1.IsPointOnSide(Side::Right, room2.GetMidpointOnSide(Side::Left))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2.GetMidpointOnSide(Side::Left);
        room1.relationRight = relation;
        room2.relationLeft = relation;
      }
      else if (room1.relationAbove == nullptr && room2.relationBelow == nullptr && room1.IsPointOnSide(Side::Top, room2.GetMidpointOnSide(Side::Bottom))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2.GetMidpointOnSide(Side::Bottom);
        room1.relationAbove = relation;
        room2.relationBelow = relation;
      }

      if (relation != nullptr) {
        relations.push_back(relation);
      }
    }
  }
}
