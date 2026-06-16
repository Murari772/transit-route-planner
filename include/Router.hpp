#pragma once
#include "Graph.hpp"
#include <vector>
#include <string>

struct PathStep {
    std::string station;
    std::string route;
    double cost;
};

class Router {
private:
    const Graph& graph;
    double transferPenalty;

public:
    explicit Router(const Graph& g, double penalty = 10.0) : graph(g), transferPenalty(penalty) {}
    std::vector<PathStep> findOptimalRoute(const std::string& start, const std::string& end);
};