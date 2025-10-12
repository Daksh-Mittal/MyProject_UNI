#ifndef PATHS_H
#define PATHS_H

#include <mcpp/mcpp.h>

struct Path {
  mcpp::Coordinate start;
  mcpp::Coordinate end;

  Path(mcpp::Coordinate start, mcpp::Coordinate end);
};

void breadth_first_search() ;

#endif
