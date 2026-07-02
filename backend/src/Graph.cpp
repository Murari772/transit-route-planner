#include "Graph.hpp"
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iostream>

void Graph::addStation(const std::string& id, const std::string& name, double lat, double lon) {
    if (stations.find(id) == stations.end()) {
        stations[id] = {id, name, lat, lon};
    }
    if (adjacencyList.find(id) == adjacencyList.end()) {
        adjacencyList[id] = std::vector<Edge>();
    }
}

void Graph::addConnection(
        const std::string& srcId, 
        const std::string& destId, 
        double weight, 
        TransitMode mode, 
        const std::string& routeName
    ) {
    // Make sure stations exist, even if dummy
    if (stations.find(srcId) == stations.end()) addStation(srcId, srcId, 0.0, 0.0);
    if (stations.find(destId) == stations.end()) addStation(destId, destId, 0.0, 0.0);

    adjacencyList[srcId].push_back({destId, weight, mode, routeName});
}

const std::vector<Edge>& Graph::getNeighbors(const std::string& stationId) const {
    static const std::vector<Edge> empty;
    auto it = adjacencyList.find(stationId);
    return (it != adjacencyList.end()) ? it->second : empty;
}

bool Graph::hasStation(const std::string& id) const {
    return stations.find(id) != stations.end();
}

bool Graph::hasTransitConnection(const std::string& stationId) const {
    auto it = adjacencyList.find(stationId);
    if (it == adjacencyList.end()) return false;

    // Check station's own edges
    for (const auto& edge : it->second) {
        if (edge.mode != TransitMode::WALK) return true;
    }

    // Check immediate neighbors reachable by 1 WALK edge
    for (const auto& edge : it->second) {
        if (edge.mode == TransitMode::WALK) {
            auto nIt = adjacencyList.find(edge.destination);
            if (nIt != adjacencyList.end()) {
                for (const auto& nEdge : nIt->second) {
                    if (nEdge.mode != TransitMode::WALK) return true;
                }
            }
        }
    }

    return false;
}

std::vector<StationMetadata> Graph::getStations() const {
    std::vector<StationMetadata> result;
    for(const auto& it : stations) {
        result.push_back(it.second);
    }
    std::sort(result.begin(), result.end(), [](const StationMetadata& a, const StationMetadata& b) {
        return a.name < b.name;
    });
    return result;
}

std::string Graph::getStationIdByName(const std::string& name) const {
    std::string bestId = "";
    int maxEdges = -1;
    for (const auto& it : stations) {
        if (it.second.name == name) {
            auto adjIt = adjacencyList.find(it.first);
            int edges = (adjIt != adjacencyList.end()) ? adjIt->second.size() : 0;
            if (edges > maxEdges) {
                maxEdges = edges;
                bestId = it.first;
            }
        }
    }
    return bestId;
}

const StationMetadata* Graph::getStation(const std::string& id) const {
    auto it = stations.find(id);
    if (it != stations.end()) return &it->second;
    return nullptr;
}

static double haversineDistanceMeters(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000.0; // Earth radius in meters
    const double PI = 3.14159265358979323846;
    
    double phi1 = lat1 * PI / 180.0;
    double phi2 = lat2 * PI / 180.0;
    double dPhi = (lat2 - lat1) * PI / 180.0;
    double dLambda = (lon2 - lon1) * PI / 180.0;
    
    double a = std::sin(dPhi / 2.0) * std::sin(dPhi / 2.0) +
               std::cos(phi1) * std::cos(phi2) *
               std::sin(dLambda / 2.0) * std::sin(dLambda / 2.0);
               
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return R * c;
}

void Graph::buildWalkingTransfers(double maxDistanceMeters, double walkSpeedKmh) {
    std::cout << "Building walking transfers... (maxDist: " << maxDistanceMeters << "m, speed: " << walkSpeedKmh << "km/h)" << std::endl;
    int edgesAdded = 0;

    std::vector<StationMetadata> st = getStations();

    for (size_t i = 0; i < st.size(); ++i) {
        for (size_t j = i + 1; j < st.size(); ++j) {
            // Ignore same logical station
            if (st[i].id == st[j].id) continue;

            double dist = haversineDistanceMeters(st[i].lat, st[i].lon, st[j].lat, st[j].lon);
            if (dist > 0.0 && dist <= maxDistanceMeters) {
                double timeMins = (dist / 1000.0) / walkSpeedKmh * 60.0;
                addConnection(st[i].id, st[j].id, timeMins, TransitMode::WALK, "WALK");
                addConnection(st[j].id, st[i].id, timeMins, TransitMode::WALK, "WALK");
                edgesAdded += 2;
            }
        }
    }
    
    std::cout << "Added " << edgesAdded << " walking transfer edges." << std::endl;
}