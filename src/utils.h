#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <mcpp/mcpp.h>
#include "models/Plot/plot.h"
#include "config.h"

void printStudentDetails();
bool isTesting(std::string testingComponent);
std::vector<Plot*> pointerisePlotVector(std::vector<Plot> plots);
void clearEntrance(Plot* plot);
void moveTo(mcpp::Coordinate from, mcpp::Coordinate to);

#endif
