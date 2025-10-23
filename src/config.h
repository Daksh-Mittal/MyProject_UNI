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

    bool IsTestMode() const;
    std::string GetTestedComponentName() const;
    int GetTestCase() const;
    mcpp::Coordinate2D* GetLocation() const;
    int GetVillageSize() const;
    int GetPlotBorder() const;
    int GetSeed() const;
    mcpp::MinecraftConnection* GetMinecraftConnection() const;

  private:
    Config();

    std::string testingComponent = "";
    mcpp::Coordinate2D* location = nullptr;
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
