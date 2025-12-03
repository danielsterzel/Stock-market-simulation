//
// Created by Daniel Sterzel on 03/12/2025.
//
#pragma once

#include <fstream>
namespace Error {
    inline void checkIfFileOpenedProperly(const std::ofstream& file, const std::string& fileName) {
        if (not file.is_open()) {
            throw std::runtime_error("File could not be opened" + fileName);
        }
    }
}
