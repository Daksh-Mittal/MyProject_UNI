#ifndef TASK_B
#define TASK_B

#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include "models/PlotRegion/plot_region.h"
#include "models/Plot/plot.h"
#include "models/Room/room.h"
#include "canvas.h"
#include "subdivision_error.h"
#include "utils.h"

std::string test_task_b();
void build_buildings(std::vector<Plot*> plots);

void build_exterior(Plot* plot, Canvas& canvas);
Floor* subdivide_interior(Plot* plot, Canvas& canvas);
void decorate_interior(Floor* floor, Canvas& canvas);

std::vector<PlotRegion> subdividePlot(const Plot& plot);
std::vector<PlotRegion> subdivide(std::vector<PlotRegion> regions, const Plot& plot);

#endif
