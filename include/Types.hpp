#pragma once
#include <string>

enum class TransitMode {
    METRO,
    BUS,
    WALK
};

struct Edge {
    std::string destination;
    double weight; 
    TransitMode mode;
    std::string routeName; 
};

struct RouteNode {
    std::string stationName;
    double accumulatedCost;
    std::string currentRoute;

    bool operator>(const RouteNode& other) const {
        return accumulatedCost > other.accumulatedCost;
    }
};