#include "Router.hpp"
#include <queue>
#include <unordered_map>
#include <algorithm>

struct ParentInfo {
    std::string station;
    std::string route;
    double cost;
    std::string previousRoute;
};

std::vector<PathStep> Router::findOptimalRoute(const std::string& start, const std::string& end) {
    if (!graph.hasStation(start) || !graph.hasStation(end)) return {};

    std::priority_queue<RouteNode, std::vector<RouteNode>, std::greater<RouteNode>> pq;
    // minCost[station][route]
    std::unordered_map<std::string, std::unordered_map<std::string, double>> minCost;
    // parentNode[station][route]
    std::unordered_map<std::string, std::unordered_map<std::string, ParentInfo>> parentNode;

    pq.push({start, 0.0, "START"});
    minCost[start]["START"] = 0.0;

    while (!pq.empty()) {
        RouteNode current = pq.top();
        pq.pop();

        if (current.stationName == end) break;

        if (current.accumulatedCost > minCost[current.stationName][current.currentRoute]) continue;

        for (const auto& edge : graph.getNeighbors(current.stationName)) {
            double penalty = 0.0;
            if (current.currentRoute != "START" && current.currentRoute != edge.routeName) {
                penalty = transferPenalty;
            }
            
            double newCost = current.accumulatedCost + edge.weight + penalty;

            if (minCost[edge.destination].find(edge.routeName) == minCost[edge.destination].end() || 
                newCost < minCost[edge.destination][edge.routeName]) {
                
                minCost[edge.destination][edge.routeName] = newCost;
                parentNode[edge.destination][edge.routeName] = {
                    current.stationName, 
                    edge.routeName, 
                    edge.weight + penalty, 
                    current.currentRoute
                };
                pq.push({edge.destination, newCost, edge.routeName});
            }
        }
    }

    if (minCost.find(end) == minCost.end() || minCost[end].empty()) return {}; // Path not found

    // Find the best route to the end station
    std::string bestEndRoute = "";
    double bestCost = -1.0;
    for (const auto& pair : minCost[end]) {
        if (bestCost < 0 || pair.second < bestCost) {
            bestCost = pair.second;
            bestEndRoute = pair.first;
        }
    }

    // Reconstruct path
    std::vector<PathStep> path;
    std::string currentStr = end;
    std::string currRoute = bestEndRoute;

    while (currentStr != start) {
        auto step = parentNode[currentStr][currRoute];
        path.push_back({currentStr, step.route, step.cost});
        currentStr = step.station;
        currRoute = step.previousRoute;
    }

    path.push_back({start, "Origin", 0.0});
    std::reverse(path.begin(), path.end());

    return path;
}