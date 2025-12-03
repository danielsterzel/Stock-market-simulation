//
// Created by Daniel Sterzel on 03/12/2025.
//

#include "ScenarioConfig.h"

// void ScenarioConfig::save() {
//
// }
void ScenarioConfig::openFile(const std::string& filepath, const bool overwriteFile) {

    // CODE DUPLICATION !!!
    // probably better to make the class that saves the scenario config
    // should this be made into a different class ???
    auto writeMode = std::ios::out;
    writeMode |= overwriteFile ? std::ios::trunc : std::ios::app;
    scenarioConfigFile.open(filepath, writeMode);

    if (scenarioConfigFile.is_open()) {

    }

}
