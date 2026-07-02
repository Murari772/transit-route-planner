#include "Router.hpp"
#include <queue>
#include <unordered_map>
#include <algorithm>

struct ParentInfo {
    std::string stationId;
    std::string route;
    TransitMode mode;
    double cost;
    std::string previousRoute;
    TransitMode previousMode;
    double appliedPenalty;
};

std::vector<PathStep> Router::findBestRoute(
    const std::string& startId, 
    const std::string& endId, 
    OptimizationCriterion criterion
) {
    if (!graph.hasStation(startId) || !graph.hasStation(endId)) return {};

    RouteNodeComparator comp(criterion);

    std::priority_queue<
        RouteNode, 
        std::vector<RouteNode>, 
        RouteNodeComparator
    > pq(comp);

    // minCost[stationId][route]
    std::unordered_map<
        std::string, 
        std::unordered_map<std::string, RouteCost>
    > minCost;

    // parentNode[stationId][route]
    std::unordered_map<
        std::string, 
        std::unordered_map<std::string, ParentInfo>
    > parentNode;

    pq.push({startId, {0.0, 0}, "START", TransitMode::WALK, false, TransitMode::WALK, ""});
    minCost[startId]["START"] = {0.0, 0};

    while (!pq.empty()) {
        RouteNode current = pq.top();
        pq.pop();

        if (current.stationId == endId) break;

        if (minCost[current.stationId][current.currentRoute].isBetterThan(current.cost, criterion)) 
            continue;

        for (const auto& edge : graph.getNeighbors(current.stationId)) {
            double penalty = 0.0;
            int addedInterchanges = 0;
            bool newHasBoarded = current.hasBoardedTransit;

            if (current.currentRoute == "WALK" && edge.routeName == "WALK") {
                continue;
            }

            TransitMode nextLastMode = current.lastTransitMode;
            std::string nextLastRoute = current.lastTransitRoute;
            if (edge.mode != TransitMode::WALK && edge.routeName != "START") {
                nextLastMode = edge.mode;
                nextLastRoute = edge.routeName;
            }

            if (current.currentRoute != edge.routeName) {
                if (edge.routeName != "WALK" && edge.routeName != "START") {
                    if (edge.mode == TransitMode::METRO) {
                        penalty = metroWaitTime;
                    } else {
                        penalty = busWaitTime;
                    }
                    
                    if (current.currentRoute == "WALK" && edge.routeName == current.lastTransitRoute) {
                        penalty += 1000.0;
                    }

                    if (current.hasBoardedTransit) {
                        addedInterchanges = 1;
                    }
                    newHasBoarded = true;
                }
            }
            
            RouteCost newCost = {
                current.cost.time + edge.weight + penalty,
                current.cost.interchanges + addedInterchanges
            };

            if (minCost[edge.destination].find(edge.routeName) == minCost[edge.destination].end() || 
                newCost.isBetterThan(minCost[edge.destination][edge.routeName], criterion)) {
                
                minCost[edge.destination][edge.routeName] = newCost;

                parentNode[edge.destination][edge.routeName] = {
                    current.stationId, 
                    edge.routeName, 
                    edge.mode,
                    edge.weight + penalty, 
                    current.currentRoute,
                    current.currentMode,
                    penalty
                };

                pq.push({edge.destination, newCost, edge.routeName, edge.mode, newHasBoarded, nextLastMode, nextLastRoute});
            }
        }
    }

    if (minCost.find(endId) == minCost.end() || minCost[endId].empty()) return {}; // Path not found

    // Find the best route to the end station
    std::string bestEndRoute = "";
    bool foundBest = false;
    RouteCost bestCost = {0.0, 0};

    for (const auto& pair : minCost[endId]) {
        if (!foundBest || pair.second.isBetterThan(bestCost, criterion)) {
            bestCost = pair.second;
            bestEndRoute = pair.first;
            foundBest = true;
        }
    }

    // Reconstruct path
    std::vector<PathStep> path;
    std::string currentStr = endId;
    std::string currRoute = bestEndRoute;
    double currentCumulativeTime = bestCost.time;

    while (currentStr != startId) {
        auto step = parentNode[currentStr][currRoute];
        path.push_back({currentStr, step.route, step.mode, currentCumulativeTime, step.appliedPenalty});
        currentStr = step.stationId;
        currRoute = step.previousRoute;
        currentCumulativeTime -= step.cost;
    }

    path.push_back({startId, "Origin", TransitMode::WALK, 0.0, 0.0});
    std::reverse(path.begin(), path.end());

    return path;
}