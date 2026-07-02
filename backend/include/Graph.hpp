#pragma once
#include "Types.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class Graph {
private:
    std::unordered_map<std::string, std::vector<Edge>> adjacencyList;
    std::unordered_map<std::string, StationMetadata> stations;

public:
    void addStation(const std::string& id, const std::string& name, double lat, double lon);

    void addConnection(
        const std::string& srcId, 
        const std::string& destId, 
        double weight, 
        TransitMode mode, 
        const std::string& routeName
    );
    
    const std::vector<Edge>& getNeighbors(const std::string& stationId) const;
    bool hasStation(const std::string& id) const;
    bool hasTransitConnection(const std::string& stationId) const;

    // Returns a list of all station metadata
    std::vector<StationMetadata> getStations() const;

    // Computes geographic distances between all stations and adds a WALK edge if within maxDistanceMeters
    void buildWalkingTransfers(double maxDistanceMeters, double walkSpeedKmh);

    // Find station ID by name
    std::string getStationIdByName(const std::string& name) const;
    
    // Get metadata by ID
    const StationMetadata* getStation(const std::string& id) const;
};