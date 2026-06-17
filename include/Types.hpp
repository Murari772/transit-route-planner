#pragma once
#include <string>

enum class TransitMode {
    METRO,
    BUS,
    WALK
};

enum class OptimizationCriterion {
    LEAST_TIME,
    LEAST_INTERCHANGES
};

struct Edge {
    std::string destination;
    double weight; 
    TransitMode mode;
    std::string routeName; 
};

struct RouteCost {
    double time;
    int interchanges;

    bool isBetterThan(const RouteCost& other, OptimizationCriterion criterion) const {
        if (criterion == OptimizationCriterion::LEAST_TIME) {
            if (time != other.time) return time < other.time;
            return interchanges < other.interchanges;
        } else {
            if (interchanges != other.interchanges) return interchanges < other.interchanges;
            return time < other.time;
        }
    }
};

struct RouteNode {
    std::string stationName;
    RouteCost cost;
    std::string currentRoute;
};