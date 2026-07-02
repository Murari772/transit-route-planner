#pragma once

#include "Router.hpp"
#include "Types.hpp"
#include "Graph.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

nlohmann::json buildRouteResponse(
    const std::string& sourceId,
    const std::string& destinationId,
    OptimizationCriterion criterion,
    const std::vector<PathStep>& path,
    const Graph& graph
);

std::string transitModeToString(TransitMode mode);
std::string criterionToString(OptimizationCriterion criterion);
OptimizationCriterion parseCriterion(const std::string& value);
