#include "Graph.hpp"
#include "Router.hpp"
#include "DataLoader.hpp"
#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char* argv[]) {
    double transferPenalty = 10.0; // Default penalty

    // Simple argument parsing for penalty
    if (argc > 1) {
        try {
            transferPenalty = std::stod(argv[1]);
        } catch (...) {
            std::cerr << "Invalid transfer penalty provided. Using default 10.0 mins.\n";
        }
    }

    Graph delhiNetwork;
    std::string dataPath = "data/metro_network.json";
    std::ifstream testFile(dataPath);
    if (!testFile) {
        dataPath = "../data/metro_network.json";
    }
    
    DataLoader::loadMetroNetwork(dataPath, delhiNetwork);

    Router router(delhiNetwork, transferPenalty);
    std::string start = "New Delhi";
    std::string end = "Mandi House";

    std::cout << "Finding route from " << start << " to " << end 
              << " with transfer penalty: " << transferPenalty << " mins...\n\n";
    auto path = router.findOptimalRoute(start, end);

    if (path.empty()) {
        std::cout << "No route found.\n";
        return 0;
    }

    double totalTime = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i == 0) {
            std::cout << "[Start] " << path[i].station << "\n";
        } else {
            bool isInterchange = (path[i-1].route != "Origin" && path[i-1].route != path[i].route);
            
            if (isInterchange) {
                std::cout << "\n*** Interchange from " << path[i-1].route << " to " << path[i].route 
                          << " (" << transferPenalty << " mins) ***\n\n";
            }
            
            double actualTravelTime = path[i].cost;
            if (isInterchange) {
                actualTravelTime -= transferPenalty;
            }

            std::cout << "  |  Take [" << path[i].route << "] -> " << path[i].station << " (" << actualTravelTime << " mins)\n";
            totalTime += path[i].cost; // We still add the full cost to the total time
        }
    }
    std::cout << "\nTotal estimated travel time: " << totalTime << " minutes.\n";

    return 0;
}