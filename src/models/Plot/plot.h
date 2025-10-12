#ifndef PLOTS_H
#define PLOTS_H

#include <mcpp/mcpp.h>

struct Plot {
  mcpp::Coordinate origin; // minimum/north-west (-x, -z) corner
  mcpp::Coordinate bound; // maximum/south-east (x, z) corner
  mcpp::Coordinate entrance; // for paths and house generation

  Plot(mcpp::Coordinate origin, mcpp::Coordinate bound, mcpp::Coordinate entrance) ;

  // additional fields and methods as needed
};

#endif
