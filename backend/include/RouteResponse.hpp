#pragma once

#include "Router.hpp"
#include "Types.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

nlohmann::json buildRouteResponse(
    const std::string& source,
    const std::string& destination,
    OptimizationCriterion criterion,
    const std::vector<PathStep>& path,
    double transferTimeMinutes
);

std::string transitModeToString(TransitMode mode);
std::string criterionToString(OptimizationCriterion criterion);
OptimizationCriterion parseCriterion(const std::string& value);
