#pragma once
#include "Graph.hpp"
#include "Types.hpp"
#include <vector>
#include <string>

class RouteNodeComparator {
    OptimizationCriterion criterion;
public:
    RouteNodeComparator(OptimizationCriterion c) : criterion(c) {}

    bool operator()(const RouteNode& a, const RouteNode& b) const {
        return b.cost.isBetterThan(a.cost, criterion);
    }
};

class Router {
private:
    const Graph& graph;
    double metroWaitTime;
    double busWaitTime;

public:
    explicit Router(const Graph& g, double metroWait = 2.0, double busWait = 5.0) 
        : graph(g), metroWaitTime(metroWait), busWaitTime(busWait) {}

    std::vector<PathStep> findBestRoute(
        const std::string& startId, 
        const std::string& endId, 
        OptimizationCriterion criterion = OptimizationCriterion::LEAST_TIME
    );
};