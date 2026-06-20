#include "RouteResponse.hpp"

std::string transitModeToString(TransitMode mode) {
    switch (mode) {
        case TransitMode::METRO: return "metro";
        case TransitMode::BUS:   return "bus";
        case TransitMode::WALK:  return "walk";
        default:                 return "unknown";
    }
}

std::string criterionToString(OptimizationCriterion criterion) {
    switch (criterion) {
        case OptimizationCriterion::LEAST_TIME:         return "least_time";
        case OptimizationCriterion::LEAST_INTERCHANGES: return "least_interchanges";
        default:                                        return "least_time";
    }
}

OptimizationCriterion parseCriterion(const std::string& value) {
    if (value == "least_interchanges" || value == "interchanges") {
        return OptimizationCriterion::LEAST_INTERCHANGES;
    }
    return OptimizationCriterion::LEAST_TIME;
}

nlohmann::json buildRouteResponse(
    const std::string& source,
    const std::string& destination,
    OptimizationCriterion criterion,
    const std::vector<PathStep>& path,
    double transferTimeMinutes
) {
    nlohmann::json response;
    response["source"] = source;
    response["destination"] = destination;
    response["criterion"] = criterionToString(criterion);

    if (path.empty()) {
        response["found"] = false;
        response["message"] = "No route found between the given stations.";
        return response;
    }

    response["found"] = true;

    nlohmann::json stations = nlohmann::json::array();
    for (const auto& step : path) {
        stations.push_back(step.station);
    }

    if (path.size() == 1) {
        response["path"] = {
            {"stations", stations},
            {"segments", nlohmann::json::array()},
            {"totalTravelTimeMinutes", 0.0},
            {"numberOfInterchanges", 0}
        };
        return response;
    }

    nlohmann::json segments = nlohmann::json::array();
    double totalTime = 0.0;
    int totalInterchanges = 0;

    std::string currentRoute = path[1].route;
    TransitMode currentMode = path[1].mode;
    std::string rideFrom = path[0].station;
    double segmentTime = 0.0;
    nlohmann::json intermediateStations = nlohmann::json::array();

    for (size_t i = 1; i < path.size(); ++i) {
        const bool isInterchange =
            path[i - 1].route != "Origin" &&
            path[i - 1].route != path[i].route;

        double actualTravelTime = path[i].cost;
        if (isInterchange) {
            actualTravelTime -= transferTimeMinutes;
        }

        if (isInterchange) {
            segments.push_back({
                {"type", "ride"},
                {"mode", transitModeToString(currentMode)},
                {"route", currentRoute},
                {"from", rideFrom},
                {"to", path[i - 1].station},
                {"travelTimeMinutes", segmentTime},
                {"intermediateStations", intermediateStations}
            });

            segments.push_back({
                {"type", "interchange"},
                {"atStation", path[i - 1].station},
                {"fromMode", transitModeToString(currentMode)},
                {"fromRoute", currentRoute},
                {"toMode", transitModeToString(path[i].mode)},
                {"toRoute", path[i].route},
                {"transferTimeMinutes", transferTimeMinutes}
            });

            totalInterchanges++;
            rideFrom = path[i - 1].station;
            segmentTime = actualTravelTime;
            currentRoute = path[i].route;
            currentMode = path[i].mode;
            intermediateStations = nlohmann::json::array();
        } else {
            if (path[i - 1].station != rideFrom) {
                intermediateStations.push_back(path[i - 1].station);
            }
            segmentTime += actualTravelTime;
        }

        totalTime += path[i].cost;
    }

    segments.push_back({
        {"type", "ride"},
        {"mode", transitModeToString(currentMode)},
        {"route", currentRoute},
        {"from", rideFrom},
        {"to", path.back().station},
        {"travelTimeMinutes", segmentTime},
        {"intermediateStations", intermediateStations}
    });

    response["path"] = {
        {"stations", stations},
        {"segments", segments},
        {"totalTravelTimeMinutes", totalTime},
        {"numberOfInterchanges", totalInterchanges}
    };

    return response;
}
