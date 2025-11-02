#include "config.h"

Config::Config() {}

Config& Config::GetInstance() {
  static Config config;
  return config;
}

void Config::ApplyConfiguration(const unsigned int argc, const char *argv[]) {
  for (unsigned int i = 0 ; i < argc ; i++) {
    std::string text = argv[i];

    if (text.length() >= 2 && (text[0] == '-' && text[1] == '-')) {
      std::string option = text.substr(2);
      std::string value;
      size_t equals = text.find('=');

      if (equals != std::string::npos) {
        option = text.substr(2, equals - 2);
        value = text.substr(equals + 1);
      }

      if (option == "component") {
        isTestMode = true;
        testingComponent = new std::string(value);
      }
      else if (option == "testmode") {
        isTestMode = true;

        // Allows shorthand - instead of '--testmode --component=abc', write '--testmode=abc'
        if (!value.empty()) {
          testingComponent = new std::string(value);
        }
      }
      else if (option == "loc") {
        size_t commaPos = value.find(',', 0);
        int x = 0;
        int z = 0;

        if (commaPos == std::string::npos) {
          throw std::invalid_argument("Expected comma for parameter 'loc'");
        }
        if (commaPos != value.rfind(',')) {
          throw std::invalid_argument("Expected single comma for parameter 'loc', received multiple commas");
        }

        try {
          x = std::stoi(value.substr(0, commaPos));
          z = std::stoi(value.substr(commaPos + 1));
        }
        catch(...) {
          throw std::invalid_argument("Expected two integers for parameter 'loc'");
        }

        // Location is parsed as x, z, using y=0 placeholder
        location = new mcpp::Coordinate2D(x, z);
      }
      else if (option == "case") {
        try {
          testCase = std::stoi(value);
        }
        catch(...) {
          throw std::invalid_argument("Expected an integer for parameter 'case'");
        }

        if (testCase <= 0) {
          throw std::invalid_argument("Parameter 'case' should be positive");
        }
      }
      else if (option == "village-size") {
        try {
          villageSize = std::stoi(value);
        }
        catch(...) {
          throw std::invalid_argument("Expected an integer for parameter 'village-size'");
        }
        if (villageSize <= 0) {
          throw std::invalid_argument("Parameter 'case' should be positive");
        }
      }
      else if (option == "plot-border") {
        try {
          plotBorder = std::stoi(value);
        }
        catch(...) {
          throw std::invalid_argument("Expected a positive integer for parameter 'plot-border'");
        }

        if (plotBorder <= 0) {
          throw std::invalid_argument("Integer cannot be negative for parameter 'plot-border'");
        }
      }
      else if (option == "seed") {
        try {
          seed = new int(std::stoi(value));
        }
        catch(...) {
          throw std::invalid_argument("Expected an integer for parameter 'seed'");
        }
      }
      else {
        std::cout << "Warning: Ignoring unknown parameter '" << option << "'" << std::endl;
      }
    }
    else {
      // we don't want the call to the executable to raise a warning
      if (i != 0) {
        std::cout << "Warning: Ignoring invalid value '" << text << "'" << std::endl;
      }
    }
  }
}

void Config::SetLocation(mcpp::Coordinate2D* newLocation) {
  if (location != nullptr) {
    delete location;
  }
  location = newLocation;
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

std::string* Config::GetTestedComponentName() const {
  return testingComponent;
}

int Config::GetTestCase() const {
  return Config::IsTestMode() ? testCase : -1;
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

Config::~Config() {
  if (location != nullptr) {
    delete location;
  }
  if (testingComponent != nullptr) {
    delete testingComponent;
  }
}
