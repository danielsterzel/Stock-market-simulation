#pragma once
#include <utility>
// TODO:
// - write mode and file opener wrapper [ X ]
// - write agent count function [ done ]


namespace CommonFunctions {
    template<typename Target, typename Container>
    decltype(auto) countObjectType(const Container& container, const Target& type) {

        int matches = 0;
        for (auto& object : container) {
            if (object and object->getType() == type) {
                ++matches;
            }
        }
        return std::pair{matches, static_cast<int>(container.size())};
    }
}
