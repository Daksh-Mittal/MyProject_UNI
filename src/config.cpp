#include "config.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <string>

// Use the correct size type for string operations
using std::string;
using string_size_type = string::size_type;

Config::Config() : villageSize(200), plotBorder(10) {}

Config& Config::GetInstance() {
  static Config config;
  return config;
}

void Config::ApplyConfiguration(std::map<string, string> config) {
  if (config.count("testmode") > 0) {
    testingComponent = config["testmode"];
    isTestMode = true;

    if (testingComponent == "") {
      testingComponent = "NOT SET";
    }
  }
  if (config.count("loc") > 0) {
    // FIX 1 & 2: Use string_size_type (or size_t) for commaPos to avoid signed/unsigned comparison errors
    string_size_type commaPos = config["loc"].find(',', 0);
    int x = 0;
    int z = 0;

    // Compare with string::npos (which is string_size_type)
    if (commaPos == string::npos) {
      throw std::invalid_argument("Expected comma for parameter 'loc'");
    }
    // Compare with rfind result (which is string_size_type)
    if (commaPos != config["loc"].rfind(',')) {
      throw std::invalid_argument("Expected single comma for parameter 'loc', received multiple commas");
    }

    try {
      x = std::stoi(config["loc"].substr(0, commaPos));
      z = std::stoi(config["loc"].substr(commaPos + 1));
    }
    catch(...) {
      throw std::invalid_argument("Expected two integers for parameter 'loc'");
    }

    if (location != nullptr) {
        delete location;
    }
    location = new mcpp::Coordinate(x, 0, z);
  }
  if (config.count("case") > 0) {
    try {
      testCase = std::stoi(config["case"]);
    }
    catch(...) {
      throw std::invalid_argument("Expected an integer for parameter 'case'");
    }
  }
  if (config.count("village-size") > 0) {
    try {
      this->villageSize = std::stoi(config["village-size"]);
    }
    catch(...) {
      throw std::invalid_argument("Expected an integer for parameter 'village-size'");
    }
  }
  if (config.count("plot-border") > 0) {
    try {
      this->plotBorder = std::stoi(config["plot-border"]);
    }
    catch(...) {
      throw std::invalid_argument("Expected an integer for parameter 'plot-border'");
    }
  }
  if (config.count("seed") > 0) {
    try {
      this->seed = new int(std::stoi(config["seed"]));
    }
    catch(...) {
      throw std::invalid_argument("Expected an integer for parameter 'seed'");
    }
  }
}

void Config::ApplyConfiguration(const int argc, const char *argv[]) {
  std::map<string, string> config;

  // FIX 3: Change loop counter i to signed int to match argc, avoiding signed/unsigned error
  for (int i = 0 ; i < argc ; i++) {
    string text = argv[i];

    if (text.length() >= 2 && (text[0] == '-' && text[1] == '-')) {
      string option = text.substr(2);
      string_size_type equals = text.find('='); // Use string_size_type for find result

      if (equals != string::npos) {
        option = text.substr(2, equals - 2);
        config[option] = text.substr(equals + 1);
      }
      else {
        config[option] = "";
      }
    }
  }

  ApplyConfiguration(config);
}

void Config::SetMinecraftConnection(mcpp::MinecraftConnection* mc) {
  this->mc = mc;
}

mcpp::MinecraftConnection* Config::GetMinecraftConnection() const {
  return mc;
}

void Config::SetLocation(mcpp::Coordinate* newLocation) {
    if (location != nullptr) {
        delete location;
    }
    location = newLocation;
}

bool Config::IsTestMode() const {
  return isTestMode;
}

string Config::GetTestedComponentName() const {
  return Config::IsTestMode() ? testingComponent : "";
}

int Config::GetTestCase() const {
  return Config::IsTestMode() ? testCase : -1;
}

mcpp::Coordinate* Config::GetLocation() const {
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

Config::~Config() {
  if (location != nullptr) {
    delete location;
  }
  if (seed != nullptr) {
      delete seed;
  }
}
