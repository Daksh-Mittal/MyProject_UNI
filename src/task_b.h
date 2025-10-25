#ifndef TASK_B
#define TASK_B

#include <vector>
#include <iostream>
#include <map>
#include "models/PlotRegion/plot_region.h"
#include "models/Plot/plot.h"
#include "models/Room/room.h"
#include "canvas.h"
#include "subdivision_error.h"
#include "utils.h"

void testTaskB();
void buildBuildings(std::vector<Plot*> plots);

void buildExterior(Plot* plot, Canvas& canvas);
Floor* subdivideInterior(Plot* plot, Canvas& canvas);
void decorateInterior(Floor* floor, Canvas& canvas);

std::vector<PlotRegion> subdividePlot(const Plot& plot);
std::vector<PlotRegion> subdivide(std::vector<PlotRegion> regions, const Plot& plot);

#endif

