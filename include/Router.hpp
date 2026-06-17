#pragma once
#include "Graph.hpp"
#include <vector>
#include <string>

struct PathStep {
    std::string station;
    std::string route;
    double cost;
};

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
    double transferPenalty;

public:
    explicit Router(const Graph& g, double penalty = 10.0) : graph(g), transferPenalty(penalty) {}
    std::vector<PathStep> findBestRoute(const std::string& start, const std::string& end, OptimizationCriterion criterion = OptimizationCriterion::LEAST_TIME);
};