#ifndef PLOTS_H
#define PLOTS_H

#include <mcpp/mcpp.h>

struct Plot {
  mcpp::Coordinate origin; 
  mcpp::Coordinate bound; 
  mcpp::Coordinate entrance; 

  Plot(mcpp::Coordinate origin, mcpp::Coordinate bound, mcpp::Coordinate entrance);
};

#endif

