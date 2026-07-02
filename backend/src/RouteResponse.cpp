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

static nlohmann::json serializeStation(const std::string& id, const Graph& graph) {
    const auto* st = graph.getStation(id);
    if (st) {
        return {
            {"id", st->id},
            {"name", st->name},
            {"lat", st->lat},
            {"lon", st->lon}
        };
    }
    return {
        {"id", id},
        {"name", id},
        {"lat", 0.0},
        {"lon", 0.0}
    };
}

nlohmann::json buildRouteResponse(
    const std::string& sourceId,
    const std::string& destinationId,
    OptimizationCriterion criterion,
    const std::vector<PathStep>& path,
    const Graph& graph
) {
    nlohmann::json response;
    
    response["source"] = serializeStation(sourceId, graph);
    response["destination"] = serializeStation(destinationId, graph);
    response["criterion"] = criterionToString(criterion);

    if (path.empty()) {
        response["found"] = false;
        response["message"] = "No route found between the given stations.";
        return response;
    }

    response["found"] = true;

    nlohmann::json stations = nlohmann::json::array();
    for (const auto& step : path) {
        stations.push_back(serializeStation(step.stationId, graph));
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
    bool hasBoardedTransit = (path[1].routeName != "WALK" && path[1].routeName != "Origin");

    std::string currentRoute = path[1].routeName;
    TransitMode currentMode = path[1].mode;
    std::string rideFromId = path[0].stationId;
    double segmentTime = 0.0;
    nlohmann::json intermediateStations = nlohmann::json::array();
    double currentWaitTime = path[1].appliedPenalty;

    for (size_t i = 1; i < path.size(); ++i) {
        const bool isInterchange =
            path[i - 1].routeName != "Origin" &&
            path[i - 1].routeName != path[i].routeName;

        bool isBoarding = false;
        if (path[i].routeName != "WALK" && path[i - 1].routeName != path[i].routeName) {
            isBoarding = true;
        }

        double actualTravelTime = path[i].cumulativeTime - path[i - 1].cumulativeTime;
        if (isBoarding) {
            actualTravelTime -= path[i].appliedPenalty;
        }

        if (isInterchange) {
            segments.push_back({
                {"type", "ride"},
                {"mode", transitModeToString(currentMode)},
                {"route", currentRoute},
                {"from", serializeStation(rideFromId, graph)},
                {"to", serializeStation(path[i - 1].stationId, graph)},
                {"travelTimeMinutes", segmentTime},
                {"intermediateStations", intermediateStations},
                {"waitTimeMinutes", currentWaitTime}
            });

            if (isBoarding && hasBoardedTransit) {
                totalInterchanges++;
            }

            if (path[i].routeName != "WALK") {
                hasBoardedTransit = true;
            }

            rideFromId = path[i - 1].stationId;
            segmentTime = actualTravelTime;
            currentRoute = path[i].routeName;
            currentMode = path[i].mode;
            currentWaitTime = path[i].appliedPenalty;
            intermediateStations = nlohmann::json::array();
        } else {
            if (path[i - 1].stationId != rideFromId) {
                intermediateStations.push_back(serializeStation(path[i - 1].stationId, graph));
            }
            segmentTime += actualTravelTime;
        }
    }
    
    totalTime = path.back().cumulativeTime;

    segments.push_back({
        {"type", "ride"},
        {"mode", transitModeToString(currentMode)},
        {"route", currentRoute},
        {"from", serializeStation(rideFromId, graph)},
        {"to", serializeStation(path.back().stationId, graph)},
        {"travelTimeMinutes", segmentTime},
        {"intermediateStations", intermediateStations},
        {"waitTimeMinutes", currentWaitTime}
    });

    response["path"] = {
        {"stations", stations},
        {"segments", segments},
        {"totalTravelTimeMinutes", totalTime},
        {"numberOfInterchanges", totalInterchanges}
    };

    return response;
}
