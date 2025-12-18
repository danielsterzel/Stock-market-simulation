//
// Created by szyro on 17.12.2025.
//

#ifndef STOCK_MARKET_SIMULATION_DATALOADER_H
#define STOCK_MARKET_SIMULATION_DATALOADER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

class DataLoader {
    public:
        static std::vector<double> loadPriceHistory(const std::string& filepath) {
            std::vector<double> prices;
            std::ifstream file(filepath);
            if (!file.is_open()) {
                std::cerr << "[WARN] Could not open data file: " << filepath << ". Simulation will use Random Walk.\n";
                return {};
            }

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                std::stringstream ss(line);
                std::string valStr;
                double price = 0.0;

                while(std::getline(ss, valStr, ',')) {
                    if(!valStr.empty()) {
                        try {
                            price = std::stod(valStr);
                        } catch (...) { continue; }
                    }
                }
                if (price > 0.0001) {
                    prices.push_back(price);
                }
            }

            std::cout << "[INFO] Loaded " << prices.size() << " price points from " << filepath << "\n";
            return prices;
        }
};


#endif //STOCK_MARKET_SIMULATION_DATALOADER_H