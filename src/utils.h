#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include "models/Plot/plot.h"
#include "config.h"

void printStudentDetails();
bool isTesting(std::string testingComponent);
std::vector<Plot*> pointerisePlotVector(std::vector<Plot> plots);

#endif

