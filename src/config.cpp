#include "config.h"

Config& Config::GetInstance() {
  static Config config;
  return config;
}

void Config::ApplyConfiguration(bool isTestMode = false, mcpp::Coordinate2D* location = nullptr, int villageSize = 200, int plotBorder = 10, int* seed = nullptr) {
  this->isTestMode = isTestMode;
  this->location = location;
  this->villageSize = villageSize;
  this->plotBorder = plotBorder;
  this->seed = seed;
}

bool Config::IsTestMode() const {
  return isTestMode;
}

mcpp::Coordinate2D* Config::GetLocation() const {
  return location;
}

int Config::GetVillageSize() const {
  return villageSize;
}

int Config::GetPlotBorder() const {
  return plotBorder;
}

int Config::GetSeed() const {
  if (seed != nullptr) {
    return *seed;
  }
  else {
    return time(nullptr);
  }
}
