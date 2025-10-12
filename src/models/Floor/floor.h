#ifndef FLOOR_H
#define FLOOR_H

#include <vector>
#include "../../enums/side.h"

// forward declaration necessary to prevent a circular dependency with Room
class Room;
class RoomRelationship;

class Floor {
  public:
    void AssignRelationships();
    void AssignRoomTypes();

    Floor(std::vector<Room> &rooms);
    ~Floor();

  private:
    std::vector<Room> *rooms;
    std::vector<RoomRelationship*> relations;
};

#endif
