#include "Graph.hpp"
#include "Router.hpp"
#include "DataLoader.hpp"
#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char* argv[]) {
    const double transferTime = 5.0;

    OptimizationCriterion criterion = OptimizationCriterion::LEAST_TIME;

    if (argc > 1) {
        if (std::string(argv[1]) == "interchanges") {
            criterion = OptimizationCriterion::LEAST_INTERCHANGES;
        }
    }

    Graph delhiNetwork;

    std::string dataPath = "data/metro_network.json";
    std::ifstream testFile(dataPath);

    if (!testFile) {
        dataPath = "../data/metro_network.json";
    }

    DataLoader::loadMetroNetwork(dataPath, delhiNetwork);

    Router router(delhiNetwork, transferTime);

    std::string start;
    std::string end;

    std::cout << "Enter source station: ";
    std::getline(std::cin >> std::ws, start);

    std::cout << "Enter destination station: ";
    std::getline(std::cin >> std::ws, end);

    std::cout << "\nFinding route from "
              << start
              << " to "
              << end;

    if (criterion == OptimizationCriterion::LEAST_INTERCHANGES) {
        std::cout << " [Searching for Least Interchanges]\n\n";
    } else {
        std::cout << " [Searching for Least Time]\n\n";
    }

    auto path = router.findBestRoute(start, end, criterion);

    if (path.empty()) {
        std::cout << "No route found.\n";
        return 0;
    }

    std::cout << "[Start] " << path[0].station << "\n";

    double totalTime = 0;
    int totalInterchanges = 0;
    double segmentTime = 0;
    std::string currentRoute;

    if (path.size() > 1) {
        currentRoute = path[1].route;
    }

    for (size_t i = 1; i < path.size(); ++i) {
        bool isInterchange =
            path[i - 1].route != "Origin" &&
            path[i - 1].route != path[i].route;

        double actualTravelTime = path[i].cost;

        if (isInterchange) {
            actualTravelTime -= transferTime;
        }

        if (isInterchange) {
            std::cout << "  |  Take [" << currentRoute
                      << "] -> "
                      << path[i - 1].station
                      << " (" << segmentTime << " mins)\n";

            std::cout << "\n*** Interchange from "
                      << currentRoute
                      << " to "
                      << path[i].route
                      << " (" << transferTime << " mins) ***\n\n";

            totalInterchanges++;
            segmentTime = actualTravelTime;
            currentRoute = path[i].route;
        } else {
            segmentTime += actualTravelTime;
        }

        totalTime += path[i].cost;
    }

    if (path.size() > 1) {
        std::cout << "  |  Take [" << currentRoute
                  << "] -> "
                  << path.back().station
                  << " (" << segmentTime << " mins)\n";
    }

    std::cout << "\nTotal estimated travel time: "
              << totalTime
              << " minutes.\n";

    std::cout << "Total interchanges: "
              << totalInterchanges
              << "\n";

    return 0;
}