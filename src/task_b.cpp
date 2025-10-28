#include "task_b.h"

void testTaskB() {
  int testCase = Config::GetInstance().GetTestCase();
  std::vector<Plot*> testPlots;
  mcpp::Coordinate origin = Config::GetInstance().GetMinecraftConnection()->getPlayerTilePosition() + mcpp::Coordinate(1, 0, 1);

  if (testCase == 1) {
    testPlots.push_back(new Plot(origin, origin + mcpp::Coordinate(13, 0, 13))); // 14x14
  }
  else if (testCase == 2) {
    testPlots.push_back(new Plot(origin, origin + mcpp::Coordinate(15, 0, 15))); // 16x16
  }
  else if (testCase == 3) {
    testPlots.push_back(new Plot(origin, origin + mcpp::Coordinate(14, 0, 18))); // 15x19
  }
  else if (testCase == 4) {
    testPlots.push_back(new Plot(origin, origin + mcpp::Coordinate(19, 0, 19))); // 20x20
  }
  else if (testCase == 5) {
    testPlots.push_back(new Plot(origin, origin + mcpp::Coordinate(19, 0, 13))); // 20x14
  }
  else {
    throw std::invalid_argument("Parameter 'case' must be an integer between 1 and 5 given component '" + Config::GetInstance().GetTestedComponentName() + "'");
  }

  buildBuildings(testPlots);

  for (Plot* plot : testPlots) {
    delete plot;
  }
}

void buildBuildings(std::vector<Plot*> plots) {
  int buildingHeight = 6;

  for (auto& plot : plots) {
    if (Config::GetInstance().IsTestMode()) {
      mcpp::Coordinate plotDimensions = plot->bound - plot->origin;
      std::cout << "Plot " << plotDimensions.x + 1 << "x" << plotDimensions.z + 1 << std::endl << std::endl;
    }

    Canvas canvas(
      std::vector<std::vector<char>> (plot->bound.z - plot->origin.z + 1, std::vector<char> (plot->bound.x - plot->origin.x + 1, ' ')), 
      // walls are composed of bricks, which we output as X
      // the upper half of the door is marked as . (ignore bottom half because modifier varies depending on orientation and x,z coordinates are the same, it doesn't matter)
      // the roof is comprised of stone slabs, which we simply output as R
      std::map<mcpp::BlockType, char, BlockTypeLess> {{mcpp::Blocks::BRICKS, 'X'}, {mcpp::BlockType(64, 8), '.'}, {mcpp::Blocks::STONE_SLAB, 'R'}, {mcpp::Blocks::OAK_WOOD_PLANK, 'G'}},
      plot->origin
    );

    plot->buildingHeight = buildingHeight ++;

    buildExterior(plot, canvas);
    Floor* floor = subdivideInterior(plot, canvas);
    decorateInterior(floor, canvas);
  }
}

void buildExterior(Plot* plot, Canvas& canvas) {
  if (isTesting("exterior")) std::cout << "Walls" << std::endl;

  canvas.Place(plot->origin, mcpp::Coordinate(plot->bound.x, plot->origin.y + plot->buildingHeight-1, plot->origin.z), mcpp::Blocks::BRICKS); // top wall
  canvas.Place(plot->origin, mcpp::Coordinate(plot->origin.x, plot->origin.y + plot->buildingHeight-1, plot->bound.z), mcpp::Blocks::BRICKS); // left wall
  canvas.Place(plot->bound, mcpp::Coordinate(plot->origin.x, plot->origin.y + plot->buildingHeight-1, plot->bound.z), mcpp::Blocks::BRICKS); // bottom wall
  canvas.Place(plot->bound, mcpp::Coordinate(plot->bound.x, plot->origin.y + plot->buildingHeight-1, plot->origin.z), mcpp::Blocks::BRICKS); // right wall
  canvas.Output("exterior"); // before roof and floor are placed

  if (isTesting("exterior")) std::cout << "Floor" << std::endl;
  canvas.Place(plot->origin, plot->bound, mcpp::Blocks::OAK_WOOD_PLANK);
  canvas.Output("exterior");

  if (isTesting("exterior")) std::cout << "Roof" << std::endl;
  canvas.Place(plot->origin + mcpp::Coordinate(0, plot->buildingHeight, 0), plot->bound + mcpp::Coordinate(0, plot->buildingHeight, 0), mcpp::Blocks::STONE_SLAB);
  canvas.Output("exterior"); // after roof and floor are placed; we expect all Xs to be replaced which is why we don't clear the canvas before doing this

  canvas.Clear();
}

Floor* subdivideInterior(Plot* plot, Canvas& canvas) {
  std::vector<Room*>* rooms = new std::vector<Room*>;
  std::vector<PlotRegion> plotRegions = subdividePlot(*plot);

  for (PlotRegion region : plotRegions) {
    Room* room = new Room(region);

    mcpp::Coordinate topLeft = room->GetCorner(Corner::TopLeft) + mcpp::Coordinate(0, 1, 0);
    mcpp::Coordinate topRight = room->GetCorner(Corner::TopRight) + mcpp::Coordinate(0, 1, 0);
    mcpp::Coordinate bottomLeft = room->GetCorner(Corner::BottomLeft) + mcpp::Coordinate(0, 1, 0);
    mcpp::Coordinate bottomRight = room->GetCorner(Corner::BottomRight) + mcpp::Coordinate(0, 1, 0);

    // note that we subtract 2 from building height because we have already built the roof by this point and have also added 1 to the y coordinate of all corners
    canvas.Place(topLeft, topRight + mcpp::Coordinate(0, plot->buildingHeight - 2, 0), mcpp::Blocks::BRICKS); // top wall
    canvas.Place(topLeft, bottomLeft + mcpp::Coordinate(0, plot->buildingHeight - 2, 0), mcpp::Blocks::BRICKS); // left wall
    canvas.Place(topRight, bottomRight + mcpp::Coordinate(0, plot->buildingHeight - 2, 0), mcpp::Blocks::BRICKS); // right wall
    canvas.Place(bottomLeft, bottomRight + mcpp::Coordinate(0, plot->buildingHeight - 2, 0), mcpp::Blocks::BRICKS); // bottom wall

    rooms->push_back(room);
  }

  Floor* floor = new Floor(rooms, plot);
  floor->AssignRelationships();

  for (auto& roomRelation : floor->GetRoomRelationships()) {
    mcpp::Coordinate coord = roomRelation->ConnectionPoint;

    roomRelation->DoorBuilt = true;

    canvas.Place(coord + mcpp::Coordinate(0, 2, 0), mcpp::BlockType(64, 8));
    canvas.Place(coord + mcpp::Coordinate(0, 1, 0), mcpp::BlockType(64, roomRelation->Orientation == Side::Top ? 1 : 0));
  }

  canvas.Output("subdivision");
  return floor;
}

void decorateInterior(Floor* floor, Canvas& canvas) {
  Plot* plot = floor->GetPlot();

  floor->ResetRoomAllocations();
  floor->AssignRoomTypes();

  // Bed
  canvas.SetRule(mcpp::BlockType(26, 8), 'B'); // two halves of a bed
  canvas.SetRule(mcpp::Blocks::BED, 'B');
  // Toilet
  canvas.SetRule(mcpp::Blocks::QUARTZ_BLOCK, 'T'); // toilet cistern
  canvas.SetRule(mcpp::BlockType(156, 7), 'T'); // toilet bowl
  // Kitchen
  canvas.SetRule(mcpp::BlockType(118, 3), 'S'); // sink
  canvas.SetRule(mcpp::Blocks::IRON_BLOCK, 'F'); // fridge
  // Storage
  canvas.SetRule(mcpp::BlockType(54, 3), 'C');
  // Library/living room
  canvas.SetRule(mcpp::Blocks::BOOKSHELF, 'K');
  
  bool hasEntrance = !plot->useDefaultEntrance;

  for (auto& room : *floor->GetRooms()) {
    RoomType roomType = room->GetRoomType();
    mcpp::Coordinate topLeftFloor = room->GetUsableCorner(Corner::TopLeft) - mcpp::Coordinate(0, 1, 0);
    mcpp::Coordinate bottomRightFloor = room->GetUsableCorner(Corner::BottomRight) - mcpp::Coordinate(0, 1, 0);
    mcpp::Coordinate entrance(0,0,0);
    bool isEntranceRoom = false;
  
    std::vector<Side> exteriorSides = room->GetExteriorSides(*plot);

    // or else each room will be too dark
    canvas.Place(room->GetCentre(plot->buildingHeight - 1), mcpp::Blocks::GLOWSTONE);

    // if no entrance is provided, use the midpoint of the left side of the first room
    // this only happens in testing. in testing, we need the entrance to be placed predictably, so this is always the midpoint of the left wall
    if (!hasEntrance && exteriorSides.size() > 0) {
      entrance = room->GetMidpointOnSide(exteriorSides.at(0));

      plot->entrance = entrance;
      exteriorSides.erase(exteriorSides.begin());
      
      canvas.Place(entrance + mcpp::Coordinate(0, 2, 0), mcpp::BlockType(64, 8));
      canvas.Place(entrance + mcpp::Coordinate(0, 1, 0), mcpp::Blocks::OAK_DOOR_BLOCK);

      hasEntrance = true;
      isEntranceRoom = true;
    }

    if (roomType == RoomType::Bedroom) {
      mcpp::Coordinate topLeft = room->GetUsableCorner(Corner::TopLeft);
      RoomRelationship* topRel = room->GetRelationship(Side::Top);
      RoomRelationship* leftRel = room->GetRelationship(Side::Left);

      // we don't want to obstruct any doorways
      if ((leftRel != nullptr && leftRel->ConnectionPoint.z == topLeft.z + 1) || (isEntranceRoom && entrance.z == topLeft.z + 1)) {
        topLeft = topLeft + mcpp::Coordinate(1, 0, 0);
      }
      if (topRel != nullptr && topRel->ConnectionPoint.x == topLeft.x) {
        topLeft = topLeft + mcpp::Coordinate(1, 0, 0);
      }

      canvas.Place(topLeft + mcpp::Coordinate(0, 0, 1), mcpp::BlockType(26, 8)); // we need a complete bed and there is no predefined first half
      canvas.Place(topLeft, mcpp::Blocks::BED);
      
      canvas.Place(topLeftFloor, bottomRightFloor, mcpp::Blocks::BLUE_WOOL);
    }
    else if (roomType == RoomType::Bathroom) {
      mcpp::Coordinate topLeft = room->GetUsableCorner(Corner::TopLeft);
      RoomRelationship* topRel = room->GetRelationship(Side::Top);
      RoomRelationship* leftRel = room->GetRelationship(Side::Left);

      // we don't want to obstruct any doorways
      if ((leftRel != nullptr && leftRel->ConnectionPoint.z == topLeft.z + 1) || (isEntranceRoom && entrance.z == topLeft.z + 1)) {
        topLeft = topLeft + mcpp::Coordinate(1, 0, 0);
      }
      if (topRel != nullptr && topRel->ConnectionPoint.x == topLeft.x) {
        topLeft = topLeft + mcpp::Coordinate(1, 0, 0);
      }

      // Toilet
      canvas.Place(topLeft, topLeft + mcpp::Coordinate(0, 1, 0), mcpp::Blocks::QUARTZ_BLOCK);
      canvas.Place(topLeft + mcpp::Coordinate(0, 0, 1), mcpp::BlockType(156, 7));
      canvas.Place(topLeft + mcpp::Coordinate(0, 1, 1), mcpp::Blocks::WEIGHTED_PRESSURE_PLATE_HEAVY);
      canvas.Place(topLeftFloor, bottomRightFloor, mcpp::Blocks::DARK_PRISMARINE);
    }
    else if (roomType == RoomType::Kitchen) {
      mcpp::Coordinate topLeft = room->GetUsableCorner(Corner::TopLeft);
      RoomRelationship* topRel = room->GetRelationship(Side::Top);

      // we don't want to obstruct the doorway at the top of the room
      if (topRel != nullptr && topRel->ConnectionPoint.x == topLeft.x + 1) {
      topLeft = topLeft + mcpp::Coordinate(2, 0, 0);
      }

      // Sink
      canvas.Place(topLeft, mcpp::BlockType(118, 3)); // 3 -> full cauldron

      // Fridge
      canvas.Place(topLeft + mcpp::Coordinate(1, 0, 0), topLeft + mcpp::Coordinate(1, 1, 0), mcpp::Blocks::IRON_BLOCK);
      canvas.Place(topLeft + mcpp::Coordinate(1, 1, 1), mcpp::BlockType(71, 8));
      canvas.Place(topLeft + mcpp::Coordinate(1, 0, 1), mcpp::BlockType(71, 1));

      canvas.Place(topLeftFloor, bottomRightFloor, mcpp::Blocks::STONE);
    }
    else if (roomType == RoomType::LivingRoom) {
      mcpp::Coordinate topLeft = room->GetUsableCorner(Corner::TopLeft);

      canvas.Place(topLeft, topLeft + mcpp::Coordinate(0,1,0), mcpp::Blocks::BOOKSHELF);
      canvas.Place(topLeftFloor, bottomRightFloor, mcpp::Blocks::RED_WOOL);
    }
    else if (roomType == RoomType::Storage) {
      mcpp::Coordinate topLeft = room->GetUsableCorner(Corner::TopLeft);

      if (!Config::GetInstance().IsTestMode()) {
        RoomRelationship* topRel = room->GetRelationship(Side::Top);

        if (topRel != nullptr && topRel->ConnectionPoint.x == topLeft.x + 1) {
          topLeft = topLeft + mcpp::Coordinate(2, 0, 0);
        }

        canvas.Place(topLeft, topLeft + mcpp::Coordinate(1,1,0), mcpp::BlockType(54, 3));
      }

      canvas.Place(topLeft, topLeft + mcpp::Coordinate(0,1,0), mcpp::BlockType(54, 3)); // chest facing south
      canvas.Place(topLeftFloor, bottomRightFloor, mcpp::Blocks::STONE);
    }

    for (Side& exteriorSide : exteriorSides) {
      mcpp::Coordinate midpoint = room->GetMidpointOnSide(exteriorSide) + mcpp::Coordinate(0, 2, 0);
      canvas.Place(midpoint, mcpp::Blocks::GLASS);
    }
  }

  if (!plot->useDefaultEntrance) {
    canvas.Place(plot->entrance + mcpp::Coordinate(0, 2, 0), mcpp::BlockType(64, 8));
    canvas.Place(plot->entrance + mcpp::Coordinate(0, 1, 0), mcpp::Blocks::OAK_DOOR_BLOCK);

    clearEntrance(plot);
  }
  
  canvas.Output("furnishing");
}

std::vector<PlotRegion> subdividePlot(const Plot& plot) {
  std::vector<PlotRegion> subdivided = subdivide(std::vector<PlotRegion>{PlotRegion{plot}}, plot);

  // we want the final plot regions to be ordered left to right, top to bottom, so that we can predict the
  // result in advance (a MUST for testing!), so we need to sort them
  // we neither need nor want any variables besides the two plotregions to be involved in the comparison between said plotregions, so the scope has been left empty
  std::sort(subdivided.begin(), subdivided.end(), [](const PlotRegion& plot1, const PlotRegion& plot2) {
    // while we could use the bottom right corner, i've simply elected to use the top left corner as the point of comparison
    mcpp::Coordinate plot1Origin = plot1.GetTopLeftCorner();
    mcpp::Coordinate plot2Origin = plot2.GetTopLeftCorner();

    return plot1Origin.z == plot2Origin.z ? plot1Origin.x < plot2Origin.x : plot1Origin.z < plot2Origin.z;
  });

  return subdivided;
}

std::vector<PlotRegion> subdivide(std::vector<PlotRegion> regions, const Plot& plot) {
  std::vector<PlotRegion> allRegions;

  for (PlotRegion& plotRegion : regions) {
    Axis axis = plotRegion.GetSubdivisionAxis();

    if (axis != Axis::None) {
      bool canMoveOn = false;
      bool didSucceed = false;

      while (!canMoveOn) {
        try {
          PlotRegion newRegion = plotRegion.Subdivide(axis, plot);
          std::vector<PlotRegion> nextRegions {plotRegion, newRegion};
          std::vector<PlotRegion> subdividedNextRegions = subdivide(nextRegions, plot);
          allRegions.insert(allRegions.end(), subdividedNextRegions.begin(), subdividedNextRegions.end());
          didSucceed = true;
          canMoveOn = true;
        }
        // FIX: Catch by constant reference
        catch(const subdivision_error& err) { 
          if (!err.CouldRandomise()) {
            canMoveOn = true;
          }
        }
      }

      if (!didSucceed) {
        allRegions.push_back(plotRegion);
      }
    }
    else {
      allRegions.push_back(plotRegion);
    }
  }

  return allRegions;
}
