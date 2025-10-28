#ifndef CONFIG_H
#define CONFIG_H

#include <mcpp/mcpp.h>
#include <map>
#include "enums/room_type.h"

class Config {
  public:
    static Config& GetInstance();
    ~Config();
    
    void ApplyConfiguration(const int argc, const char *argv[]);
    void ApplyConfiguration(std::map<std::string, std::string> config);
    void SetMinecraftConnection(mcpp::MinecraftConnection* mc);
    void SetLocation(mcpp::Coordinate* location); // ADDED: Setter for default location

    bool IsTestMode() const;
    std::string GetTestedComponentName() const;
    int GetTestCase() const;
    mcpp::Coordinate* GetLocation() const; // Changed from mcpp::Coordinate2D* to mcpp::Coordinate* for consistency
    int GetVillageSize() const;
    int GetPlotBorder() const;
    int GetSeed() const;
    mcpp::MinecraftConnection* GetMinecraftConnection() const;

  private:
    Config();

    std::string testingComponent = "";
    mcpp::Coordinate* location = nullptr; // Adjusted type for clarity, though it seems it was intended to store 3D coords
    mcpp::MinecraftConnection* mc = nullptr;
    bool isTestMode = false;
    int testCase = -1;
    int villageSize = 0;
    int plotBorder = 0;
    int* seed = nullptr;

    // config is a singleton, so we must disallow copying and assignment
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif
