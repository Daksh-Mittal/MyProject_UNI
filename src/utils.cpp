#include "utils.h"

void printStudentDetails() {
  std::cout << "Student 1 - Task A: Daksh Mittal - S4075055 - @Daksh-Mittal " << std::endl ;
  std::cout << "Student 2 - Task B: Brandon Tan - S4167619 - @bt-uni " << std::endl ;
  std::cout << "Student 3 - Task C: Jacob Dong - S4172023 - @JDONNn " << std::endl ;
}

bool isTesting(std::string testingComponent) {
  bool isBeingTested = false;
  std::string* configuredTestingComponent = Config::GetInstance().GetTestedComponentName();

  if (testingComponent == "") {
    isBeingTested = true;
  }
  else if (configuredTestingComponent != nullptr && *configuredTestingComponent == testingComponent) {
    isBeingTested = true;
  }

  return Config::GetInstance().IsTestMode() && isBeingTested;
} 

std::vector<Plot*> pointerisePlotVector(std::vector<Plot> plots) {
  std::vector<Plot*> plotPointers;

  for (auto& plot : plots) {
    plotPointers.push_back(new Plot(plot));
  }

  return plotPointers;
}

void moveTo(mcpp::Coordinate from, mcpp::Coordinate to) {
  mcpp::MinecraftConnection* mc = Config::GetInstance().GetMinecraftConnection();
  mcpp::BlockType block = mc->getBlock(from);
  
  mc->setBlock(from, mcpp::Blocks::AIR);
  mc->setBlock(to, block);
}

void clearEntrance(Plot* plot) {
  mcpp::MinecraftConnection* mc = Config::GetInstance().GetMinecraftConnection();
  mcpp::Coordinate entrance = plot->entrance + mcpp::Coordinate(0, 1, 0);

  // Left
  if (entrance.x == plot->origin.x) {
    mcpp::Coordinate check = entrance + mcpp::Coordinate(1, 0, 0);

    if (mc->getBlock(check) != mcpp::Blocks::AIR) {
      moveTo(check, check + mcpp::Coordinate(0, 0, 1));
      moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(0, 1, 1));

      check = check + mcpp::Coordinate(1, 0, 0);

      if (mc->getBlock(check) != mcpp::Blocks::AIR) {
        moveTo(check, check + mcpp::Coordinate(0, 0, 1));
        moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(0, 1, 1));
      }
    }
  }
  // Right
  else if (entrance.x == plot->bound.x) {
    mcpp::Coordinate check = entrance + mcpp::Coordinate(-1, 0, 0);

    if (mc->getBlock(check) != mcpp::Blocks::AIR) {
      moveTo(check, check + mcpp::Coordinate(0, 0, 1));
      moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(0, 1, 1));

      check = check + mcpp::Coordinate(-1, 0, 0);

      if (mc->getBlock(check) != mcpp::Blocks::AIR) {
        moveTo(check, check + mcpp::Coordinate(0, 0, 1));
        moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(0, 1, 1));
      }
    }
  }
  // Top
  else if (entrance.z == plot->origin.z) {
    mcpp::Coordinate check = entrance + mcpp::Coordinate(0, 0, 1);

    if (mc->getBlock(check) != mcpp::Blocks::AIR) {
      moveTo(check, check + mcpp::Coordinate(1, 0, 0));
      moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(1, 1, 0));

      check = check + mcpp::Coordinate(0, 0, 1);

      if (mc->getBlock(check) != mcpp::Blocks::AIR) {
        moveTo(check, check + mcpp::Coordinate(1, 0, 0));
        moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(1, 1, 0));
      }
    }
  }
  // Bottom
  else if (entrance.z == plot->bound.z) {
    mcpp::Coordinate check = entrance + mcpp::Coordinate(0, 0, -1);

    if (mc->getBlock(check) != mcpp::Blocks::AIR) {
      moveTo(check, check + mcpp::Coordinate(1, 0, 0));
      moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(1, 1, 0));

      check = check + mcpp::Coordinate(0, 0, -1);

      if (mc->getBlock(check) != mcpp::Blocks::AIR) {
        moveTo(check, check + mcpp::Coordinate(1, 0, 0));
        moveTo(check + mcpp::Coordinate(0, 1, 0), check + mcpp::Coordinate(1, 1, 0));
      }
    }
  }
}
