#ifndef CONFIG_H
#define CONFIG_H

#include <mcpp/mcpp.h>
#include <map>
#include <iostream>
#include <exception>
#include "enums/room_type.h"

class Config {
  public:
    static Config& GetInstance();
    ~Config();
    
    void ApplyConfiguration(const int argc, const char *argv[]);
    void SetMinecraftConnection(mcpp::MinecraftConnection* mc);
    void SetLocation(mcpp::Coordinate2D* location); // Setter for default location

    bool IsTestMode() const;
    std::string* GetTestedComponentName() const;
    int GetTestCase() const;
    mcpp::Coordinate2D* GetLocation() const; 
    int GetVillageSize() const;
    int GetPlotBorder() const;
    int GetSeed() const;
    mcpp::MinecraftConnection* GetMinecraftConnection() const;

  private:
    Config();

    std::string* testingComponent = nullptr;
    mcpp::Coordinate2D* location = nullptr;
    mcpp::MinecraftConnection* mc = nullptr;
    bool isTestMode = false;
    int testCase = -1;
    int villageSize = 200;
    int plotBorder = 10;
    int* seed = nullptr;

    // config is a singleton, so we must disallow copying and assignment
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif
