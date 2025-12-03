#pragma once
#include <fstream>

class ScenarioConfig {
    public:
    ScenarioConfig() = default;

    void openFile(const std::string& filepath, bool overwriteFile = false);
    void save();

    ~ScenarioConfig() = default;
private:
    void saveScenarioConfig();
    std::ofstream scenarioConfigFile;

};
