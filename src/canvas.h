#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <map>
#include <mcpp/mcpp.h>
#include <iostream>
#include "config.h"
#include "utils.h"

// functor needed for std::map<mcpp::BlockType, ..., BlockTypeLess> since mcpp::BlockType lacks an operator definition for <
struct BlockTypeLess {
  bool operator() (const mcpp::BlockType& blockLeft, const mcpp::BlockType& blockRight) const;
};

class Canvas {
  public:
    void Place(mcpp::Coordinate point, mcpp::BlockType block);
    void Place(mcpp::Coordinate start, mcpp::Coordinate end, mcpp::BlockType block);
    void Output(std::string testingComponent = "") const;
    void Clear();
    void SetRule(mcpp::BlockType block, char output);

    // grid must be large enough to support all block placements – undefined behaviour if this condition is not met
    // origin should be Plot.origin
    Canvas(std::vector<std::vector<char>> grid, std::map<mcpp::BlockType, char, BlockTypeLess> rules, mcpp::Coordinate origin);

  private:
    std::vector<std::vector<char>> grid;
    std::map<mcpp::BlockType, char, BlockTypeLess> rules;
    mcpp::Coordinate origin;
};

#endif
