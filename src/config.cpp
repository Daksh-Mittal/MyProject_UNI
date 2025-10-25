#include "config.h"
#include <stdexcept>

Config::Config() {}

Config& Config::GetInstance() {
  static Config config;
  return config;
}

void Config::ApplyConfiguration(std::map<std::string, std::string> config) {
  if (config.count("testmode") > 0) {
    testingComponent = config["testmode"];
    isTestMode = true;

    if (testingComponent == "") {
      testingComponent = "NOT SPECIFIED"; // OK since there will never be a space in a user-provided options
    }
  }
  if (config.count("loc") > 0) {
    int commaPos = config["loc"].find(',', 0);
    int x = 0;
    int z = 0;

    if (commaPos == std::string::npos) {
      throw std::invalid_argument("Expected comma for parameter 'loc'");
    }
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

    // Location is parsed as x, z, using y=0 placeholder
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
  std::map<std::string, std::string> config;
  std::string lastKey = "";

  for (unsigned int i = 0 ; i < argc ; i++) {
    std::string text = argv[i];

    if (text.length() >= 2 && (text[0] == '-' && text[1] == '-')) {
      std::string option = text.substr(2);

      lastKey = option;
      config[option] = ""; // if no value is passed, we still want the option to be detected
    }
    else if (lastKey != "") {
      config[lastKey] = text;
      lastKey = "";
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

bool Config::IsTestMode() const {
  return isTestMode;
}

std::string Config::GetTestedComponentName() const {
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
}
