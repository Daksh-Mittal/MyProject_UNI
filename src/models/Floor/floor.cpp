#include "floor.h"
#include "../RoomRelationship/room_relationship.h"
#include "../Room/room.h"
#include <cmath> // ADDED: Includes std::floor and std::ceil

Floor::Floor(std::vector<Room*>* rooms, Plot* plot) : rooms(rooms), plot(plot) {}

// we must clean up RoomRelationships since they are loaded into memory by 'new'
// we also clean up Rooms because we consider Floor the owner of each room object (once Floor is no longer needed, we assume each Room is no longer needed either)
Floor::~Floor() {
  for (auto& relation : relations) {
    delete relation;
  }
  if (rooms != nullptr) {
    for (auto& room : *rooms) {
      delete room;
    }
    delete rooms;
  }
}

void Floor::ResetRoomAllocations() {
  if (roomAllocations.size() > 0) roomAllocations.clear();

  // In test mode, roomAllocations acts as a counter for rooms
  // Outside of test mode, roomAllocations specifies how many of each room can be placed
  if (Config::GetInstance().IsTestMode()) {
    // Because the rooms are allocated in this order, in test mode, we expect a pattern of Bedroom, Bathroom, Living Room,
    // Storage, Kitchen, Bedroom, etc...
    for (auto& roomType : {RoomType::Bedroom, RoomType::Bathroom, RoomType::LivingRoom, RoomType::Storage, RoomType::Kitchen}) {
      roomAllocations[roomType] = 0;
    }
  }
  else {
    int numRooms = rooms->size();
    int bedrooms = std::max(1, static_cast<int>(std::floor(numRooms / 3)));

    roomAllocations[RoomType::Bedroom] = bedrooms;
    roomAllocations[RoomType::Bathroom] = std::max(1, bedrooms);
    roomAllocations[RoomType::LivingRoom] = std::max(1, static_cast<int>(std::ceil(bedrooms / 2)));
    roomAllocations[RoomType::Storage] = std::max(1, static_cast<int>(std::floor(bedrooms / 3)));
    roomAllocations[RoomType::Kitchen] = std::max(1, static_cast<int>(std::ceil(bedrooms / 3)));
  }
}

RoomType Floor::SelectRoomType() const {
  std::vector<RoomType> roomTypes;
  RoomType selectedType;

  // In test mode, we must iterate sequentially over all room types such that each room is chosen with as little repetition as possible
  // Outside of test mode, we randomly select rooms
  if (Config::GetInstance().IsTestMode()) {
    int max = -1;

    for (auto& roomType : {RoomType::Bedroom, RoomType::Bathroom, RoomType::LivingRoom, RoomType::Storage, RoomType::Kitchen}) {
      if (max == -1 || roomAllocations.at(roomType) < max) {
        max = roomAllocations.at(roomType);
        selectedType = roomType;
      }
    }
  }
  else {
    for (auto& [roomType, val] : roomAllocations) {
      if (val > 0) {
        roomTypes.push_back(roomType);
      }
    }

    selectedType = roomTypes.size() > 0 ? roomTypes[std::rand() % roomTypes.size()] : RoomType::Bedroom; // we want bedroom to be the fallback room allocation
  }

  return selectedType;
}

void Floor::AssignRoomTypes() {
  for (auto& room : (*rooms)) {
    RoomType selectedType = SelectRoomType();

    // in test mode, acts as a counter. outside test mode, specifies number of each room type available (and is thus decremented)
    if (Config::GetInstance().IsTestMode()) {
      roomAllocations[selectedType]++;
    }
    else {
      roomAllocations[selectedType]--;
    }

    room->SetRoomType(selectedType);
  }
}

void Floor::AssignRelationships() {
  for (size_t i = 0; i < rooms->size(); ++i) {
    for (size_t j = 0; j < rooms->size(); ++j) {
      // preferably we'd start from j=i+1 to avoid duplicates and self-comparison, but it's not easy to intuitively perform comparison logic
      // and the comparisons below are assymetric so this works best
      Room* room1 = (*rooms)[i];
      Room* room2 = (*rooms)[j];

      RoomRelationship* relation = nullptr;

      if (room1->relationBelow == nullptr && room2->relationAbove == nullptr && room1->IsPointOnSide(Side::Bottom, room2->GetMidpointOnSide(Side::Top))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2->GetMidpointOnSide(Side::Top);
        room1->relationBelow = relation;
        room2->relationAbove = relation;
        relation->Orientation = Side::Top;
      }
      else if (room1->relationLeft == nullptr && room2->relationRight == nullptr && room1->IsPointOnSide(Side::Left, room2->GetMidpointOnSide(Side::Right))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2->GetMidpointOnSide(Side::Right);
        room1->relationLeft = relation;
        room2->relationRight = relation;
        relation->Orientation = Side::Left;
      }
      else if (room1->relationRight == nullptr && room2->relationLeft == nullptr && room1->IsPointOnSide(Side::Right, room2->GetMidpointOnSide(Side::Left))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2->GetMidpointOnSide(Side::Left);
        room1->relationRight = relation;
        room2->relationLeft = relation;
        relation->Orientation = Side::Left;
      }
      else if (room1->relationAbove == nullptr && room2->relationBelow == nullptr && room1->IsPointOnSide(Side::Top, room2->GetMidpointOnSide(Side::Bottom))) {
        relation = new RoomRelationship(room1, room2);
        relation->ConnectionPoint = room2->GetMidpointOnSide(Side::Bottom);
        room1->relationAbove = relation;
        room2->relationBelow = relation;
        relation->Orientation = Side::Top;
      }

      if (relation != nullptr) {
        relations.push_back(relation);
      }
    }
  }
}

std::vector<RoomRelationship*> Floor::GetRoomRelationships() const {
  return relations;
}

std::vector<Room*> Floor::GetRoomsByType(RoomType type) const {
  std::vector<Room*> matchingRooms;

  for (auto& room : *rooms) {
    if (room->GetRoomType() == type) {
      matchingRooms.push_back(room);
    }
  }

  return matchingRooms;
}

Plot* Floor::GetPlot() const {
  return plot;
}

std::vector<Room*>* Floor::GetRooms() const {
  return rooms;
}
