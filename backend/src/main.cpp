#include "Graph.hpp"
#include "Router.hpp"
#include "DataLoader.hpp"
#include "RouteResponse.hpp"
#include "CrowApp.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr double kTransferTimeMinutes = 5.0;

Graph loadNetwork() {
    Graph network;
    const std::vector<std::vector<std::string>> dataPathGroups = {
        {"data/metro_network.json", "data/bus_routes.json"},
        {"../data/metro_network.json", "../data/bus_routes.json"}
    };

    for (const auto& group : dataPathGroups) {
        std::ifstream probe(group.front());
        if (!probe) {
            continue;
        }

        for (const auto& dataPath : group) {
            DataLoader::loadMetroNetwork(dataPath, network);
        }
        return network;
    }

    std::cerr << "Could not locate network data files.\n";
    return network;
}

crow::response makeJsonResponse(int code, const nlohmann::json& body) {
    crow::response res(code, body.dump());
    res.add_header("Content-Type", "application/json");
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
}

crow::response handleRouteRequest(
    const std::string& source,
    const std::string& destination,
    const std::string& criterionStr,
    Router& router
) {
    if (source.empty() || destination.empty()) {
        return makeJsonResponse(400, {
            {"error", "Both source and destination are required."}
        });
    }

    const OptimizationCriterion criterion = parseCriterion(criterionStr);
    const auto path = router.findBestRoute(source, destination, criterion);
    const auto body = buildRouteResponse(
        source,
        destination,
        criterion,
        path,
        kTransferTimeMinutes
    );

    return makeJsonResponse(path.empty() ? 404 : 200, body);
}
} // namespace

int main() {
    Graph delhiNetwork = loadNetwork();
    Router router(delhiNetwork, kTransferTimeMinutes);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/health")
    ([] {
        return makeJsonResponse(200, {{"status", "ok"}});
    });

    CROW_ROUTE(app, "/route")
    ([&router](const crow::request& req) {
        const char* source = req.url_params.get("source");
        const char* destination = req.url_params.get("destination");
        const char* criterion = req.url_params.get("criterion");

        if (!source || !destination) {
            return makeJsonResponse(400, {
                {"error", "Query parameters 'source' and 'destination' are required."}
            });
        }

        return handleRouteRequest(
            source,
            destination,
            criterion ? criterion : "least_time",
            router
        );
    });

    CROW_ROUTE(app, "/route").methods(crow::HTTPMethod::Post)
    ([&router](const crow::request& req) {
        try {
            const auto body = nlohmann::json::parse(req.body);

            if (!body.contains("source") || !body.contains("destination")) {
                return makeJsonResponse(400, {
                    {"error", "JSON body must include 'source' and 'destination'."}
                });
            }

            const std::string criterion = body.value("criterion", "least_time");
            return handleRouteRequest(
                body["source"].get<std::string>(),
                body["destination"].get<std::string>(),
                criterion,
                router
            );
        } catch (const nlohmann::json::exception&) {
            return makeJsonResponse(400, {
                {"error", "Invalid JSON body."}
            });
        }
    });

    const int port = 8080;
    std::cout << "Metro Route Planner API listening on http://0.0.0.0:" << port << "\n";
    std::cout << "GET  /route?source=<station>&destination=<station>&criterion=least_time|least_interchanges\n";
    std::cout << "POST /route  {\"source\":\"...\",\"destination\":\"...\",\"criterion\":\"least_time\"}\n";

    app.port(port).multithreaded().run();
    return 0;
}
