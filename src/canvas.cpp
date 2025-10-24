#include "canvas.h"

Canvas::Canvas(std::vector<std::vector<char>> grid, std::map<mcpp::BlockType, char, BlockTypeLess> rules, mcpp::Coordinate origin) : grid(grid), rules(rules), origin(origin) {}

void Canvas::Place(mcpp::Coordinate point, mcpp::BlockType block) {
  mcpp::MinecraftConnection* mc = Config::GetInstance().GetMinecraftConnection();
  mc->setBlock(point, block);

  if (Config::GetInstance().IsTestMode() && rules.count(block) > 0) {
    grid[point.z - origin.z][point.x - origin.x] = rules[block];
  }
}

void Canvas::Place(mcpp::Coordinate start, mcpp::Coordinate end, mcpp::BlockType block) {
  mcpp::MinecraftConnection* mc = Config::GetInstance().GetMinecraftConnection();
  mc->setBlocks(start, end, block);

  // we don't want to do anything else if the program is not in test mode, there is no rule for a given block
  if (Config::GetInstance().IsTestMode() && rules.count(block) == 1) {
    char outputChar = rules[block];

    if (start.x == end.x) {
      // vertical
      int from = std::min(start.z - origin.z, end.z - origin.z);
      int to = std::max(start.z - origin.z, end.z - origin.z);

      for (int y = from; y <= to; ++y) {
        grid[y][start.x - origin.x] = outputChar;
      }
    }
    else if (start.z == end.z) {
      // horizontal
      int from = std::min(start.x - origin.x, end.x - origin.x);
      int to = std::max(start.x - origin.x, end.x - origin.x);

      for (int x = from; x <= to; ++x) {
        grid[start.z - origin.z][x] = outputChar;
      }
    }
    else {
      int xfrom = std::min(start.x - origin.x, end.x - origin.x);
      int xto = std::max(start.x - origin.x, end.x - origin.x);
      int zfrom = std::min(start.z - origin.z, end.z - origin.z);
      int zto = std::max(start.z - origin.z, end.z - origin.z);

      for (int z = zfrom; z <= zto; ++z) {
        for (int x = xfrom; x<= xto; ++x) {
          grid[z][x] = outputChar;
        }
      }
    }
  }
}

void Canvas::Output(std::string testingComponent) const {
  if (isTesting(testingComponent)) {
    std::cout << std::endl; // lead with blank lines

    for (auto& line : grid) {
      for (auto& outputChar : line) {
        std::cout << outputChar;
      }
      std::cout << std::endl;
    }

    std::cout << std::endl << std::endl; // trail with two blank lines (we do this for the sake of clarity)
  }
}

void Canvas::Clear() {
  for (auto& line : grid) {
    std::fill(line.begin(), line.end(), ' ');
  }
}

void Canvas::SetRule(mcpp::BlockType block, char output) {
  rules[block] = output;
}

// functor needed for std::map<mcpp::BlockType, ..., BlockTypeLess> since mcpp::BlockType lacks an operator definition for <
bool BlockTypeLess::operator() (const mcpp::BlockType& blockLeft, const mcpp::BlockType& blockRight) const {
  return (blockLeft.id == blockRight.id) ? (blockLeft.mod < blockRight.mod) : (blockLeft.id < blockRight.id);
}
