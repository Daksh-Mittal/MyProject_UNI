#ifndef CONFIG_H
#define CONFIG_H

#include <mcpp/mcpp.h>

// forward declaration
class Config;

class Config {
  public:
    static Config& GetInstance();
    
    void ApplyConfiguration(bool isTestMode, mcpp::Coordinate2D* location, int villageSize, int plotBorder, int* seed);

    bool IsTestMode() const;
    mcpp::Coordinate2D* GetLocation() const;
    int GetVillageSize() const;
    int GetPlotBorder() const;
    int GetSeed() const;

  private:
    Config();

    bool isTestMode;
    mcpp::Coordinate2D* location;
    int villageSize;
    int plotBorder;
    int* seed;

    // config is a singleton, so we must disallow copying and assignment
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif
