#include "Graph.hpp"

void Graph::addStation(const std::string& name) {
    if (adjacencyList.find(name) == adjacencyList.end()) {
        adjacencyList[name] = std::vector<Edge>();
    }
}

void Graph::addConnection(
        const std::string& src, 
        const std::string& dest, 
        double weight, 
        TransitMode mode, 
        const std::string& routeName
    ) {
    addStation(src);
    addStation(dest);

    adjacencyList[src].push_back({dest, weight, mode, routeName});
    adjacencyList[dest].push_back({src, weight, mode, routeName});
}

const std::vector<Edge>& Graph::getNeighbors(const std::string& station) const {
    static const std::vector<Edge> empty;
    auto it = adjacencyList.find(station);
    return (it != adjacencyList.end()) ? it->second : empty;
}

bool Graph::hasStation(const std::string& name) const {
    return adjacencyList.find(name) != adjacencyList.end();
}