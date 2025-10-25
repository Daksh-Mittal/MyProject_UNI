#ifndef FLOOR_H
#define FLOOR_H

#include <vector>
#include <map>
#include "enums/side.h"
#include "enums/room_type.h"
#include "models/Plot/plot.h"
#include "config.h"

// forward declaration necessary to prevent a circular dependency with Room
class Room;
struct RoomRelationship;

class Floor {
  public:
    void AssignRelationships();
    void AssignRoomTypes();
    void ResetRoomAllocations();
    Plot* GetPlot() const;
    std::vector<RoomRelationship*> GetRoomRelationships() const;
    std::vector<Room*> GetRoomsByType(RoomType) const;
    std::vector<Room*>* GetRooms() const;
    RoomType SelectRoomType() const;

    Floor(std::vector<Room*>* rooms, Plot* plot);
    ~Floor();

  private:
    std::vector<Room*>* rooms;
    Plot* plot;
    std::vector<RoomRelationship*> relations;
    std::map<RoomType, int> roomAllocations;
};

#endif
