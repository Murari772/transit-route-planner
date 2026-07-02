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

struct StationMetadata {
    std::string id;
    std::string name;
    double lat;
    double lon;
};

struct Edge {
    std::string destination; // Now holds station ID
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
        } 
        else {
            if (interchanges != other.interchanges) return interchanges < other.interchanges;
            return time < other.time;
        }
    }
};

struct RouteNode {
    std::string stationId;
    RouteCost cost;
    std::string currentRoute;
    TransitMode currentMode;
    bool hasBoardedTransit;
    TransitMode lastTransitMode;
    std::string lastTransitRoute;
};

// Represents a step in the final path
struct PathStep {
    std::string stationId;
    std::string routeName;
    TransitMode mode;
    double cumulativeTime;
    double appliedPenalty;
};