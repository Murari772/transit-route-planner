#include "Graph.hpp"
#include "Router.hpp"
#include "GTFSParser.hpp"
#include "RouteResponse.hpp"
#include "CrowApp.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <memory>

namespace fs = std::filesystem;

namespace {
std::string trim(const std::string& s) {
    const size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }

    const size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

constexpr double kMetroWaitTime = 2.0;
constexpr double kBusWaitTime = 5.0;

std::unordered_map<std::string, std::shared_ptr<Router>> routers;
std::unordered_map<std::string, std::shared_ptr<Graph>> graphs;
std::vector<std::string> availableCities;

void loadNetworks() {
    std::string basePath = "data/gtfs";
    if (!fs::exists(basePath)) {
        basePath = "../data/gtfs";
    }

    if (!fs::exists(basePath)) {
        std::cerr << "GTFS directory not found at " << basePath << "\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(basePath)) {
        if (entry.is_directory()) {
            std::string city = entry.path().filename().string();
            std::cout << "Discovered city: " << city << "\n";
            
            auto graph = std::make_shared<Graph>();
            
            // Iterate through sub-folders (providers like bus, metro)
            for (const auto& providerEntry : fs::directory_iterator(entry.path())) {
                if (providerEntry.is_directory()) {
                    std::string provider = providerEntry.path().filename().string();
                    std::cout << "  Loading feed: " << provider << "\n";
                    TransitMode defaultMode = TransitMode::METRO;
                    if (provider == "bus" || provider == "Bus") defaultMode = TransitMode::BUS;
                    GTFSParser::parse(providerEntry.path().string(), *graph, defaultMode, provider + "_");
                }
            }
            
            graph->buildWalkingTransfers(500.0, 5.0);
            
            graphs[city] = graph;
            routers[city] = std::make_shared<Router>(*graph, kMetroWaitTime, kBusWaitTime);
            availableCities.push_back(city);
        }
    }
}

crow::response makeJsonResponse(int code, const nlohmann::json& body) {
    crow::response res(code, body.dump());
    res.add_header("Content-Type", "application/json");
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

crow::response handleRouteRequest(
    const std::string& city,
    const std::string& sourceName,
    const std::string& destinationName,
    const std::string& criterionStr
) {
    if (routers.find(city) == routers.end()) {
        return makeJsonResponse(404, {{"error", "City not found."}});
    }

    if (sourceName.empty() || destinationName.empty()) {
        return makeJsonResponse(400, {
            {"error", "Both source and destination are required."}
        });
    }

    auto& graph = *graphs[city];
    auto& router = *routers[city];

    std::string sourceId = graph.getStationIdByName(sourceName);
    std::string destId = graph.getStationIdByName(destinationName);

    if (sourceId.empty() || destId.empty()) {
         return makeJsonResponse(400, {
            {"error", "Source or destination station not found."}
        });
    }

    const OptimizationCriterion criterion = parseCriterion(criterionStr);
    const auto path = router.findBestRoute(sourceId, destId, criterion);
    const auto body = buildRouteResponse(
        sourceId,
        destId,
        criterion,
        path,
        graph
    );

    return makeJsonResponse(path.empty() ? 404 : 200, body);
}
} // namespace

int main() {
    loadNetworks();

    if (availableCities.empty()) {
        std::cerr << "Warning: No cities loaded.\n";
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/health")
    ([] {
        return makeJsonResponse(200, {{"status", "ok"}});
    });

    CROW_ROUTE(app, "/cities")
    .methods(crow::HTTPMethod::Get)
    ([]() {
        nlohmann::json response = availableCities;
        return makeJsonResponse(200, response);
    });

    CROW_ROUTE(app, "/route")
    .methods(crow::HTTPMethod::Get)
    ([](const crow::request& req) {
        const char* cityParam = req.url_params.get("city");
        const char* sourceParam = req.url_params.get("source");
        const char* destinationParam = req.url_params.get("destination");
        const char* criterion = req.url_params.get("criterion");

        if (!cityParam || !sourceParam || !destinationParam) {
            return makeJsonResponse(400, {
                {"error", "Query parameters 'city', 'source', and 'destination' are required."}
            });
        }

        const std::string city = trim(cityParam);
        const std::string source = trim(sourceParam);
        const std::string destination = trim(destinationParam);

        std::cout << "City: [" << city << "] Source: [" << source << "] Dest: [" << destination << "]\n";

        return handleRouteRequest(
            city,
            source,
            destination,
            criterion ? criterion : "least_time"
        );
    });

    CROW_ROUTE(app, "/stations")
    .methods(crow::HTTPMethod::Get)
    ([](const crow::request& req) {
        const char* cityParam = req.url_params.get("city");
        if (!cityParam) {
            return makeJsonResponse(400, {{"error", "Query parameter 'city' is required."}});
        }

        std::string city = trim(cityParam);
        if (graphs.find(city) == graphs.end()) {
            return makeJsonResponse(404, {{"error", "City not found."}});
        }

        auto stations = graphs[city]->getStations();
        std::vector<std::string> names;
        for (const auto& st : stations) {
            if (graphs[city]->hasTransitConnection(st.id)) {
                names.push_back(st.name);
            }
        }

        std::sort(names.begin(), names.end());
        names.erase(std::unique(names.begin(), names.end()), names.end());

        nlohmann::json response = names;
        return makeJsonResponse(200, response);
    });

    const int port = 8080;
    std::cout << "Metro Route Planner API listening on http://0.0.0.0:" << port << "\n";

    app.port(port).multithreaded().run();
    return 0;
}
