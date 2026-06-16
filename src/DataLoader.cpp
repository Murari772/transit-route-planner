#include "DataLoader.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void DataLoader::loadMetroNetwork(const std::string& filepath, Graph& graph) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    for (const auto& item : j) {
        std::string source = item["source"];
        std::string dest = item["destination"];
        double weight = item["weight"];
        std::string modeStr = item["mode"];
        std::string route = item["route"];

        TransitMode mode = TransitMode::METRO;
        if (modeStr == "BUS") mode = TransitMode::BUS;
        else if (modeStr == "WALK") mode = TransitMode::WALK;

        graph.addConnection(source, dest, weight, mode, route);
    }
}
