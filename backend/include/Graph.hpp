#pragma once
#include "Types.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class Graph {
private:
    std::unordered_map<std::string, std::vector<Edge>> adjacencyList;

public:
    void addStation(const std::string& name);

    void addConnection(
        const std::string& src, 
        const std::string& dest, 
        double weight, 
        TransitMode mode, 
        const std::string& routeName
    );
    
    const std::vector<Edge>& getNeighbors(const std::string& station) const;
    bool hasStation(const std::string& name) const;
};