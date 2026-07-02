#include "GTFSParser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>

namespace {
std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::string current = "";
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            result.push_back(current);
            current = "";
        } else {
            current += c;
        }
    }
    result.push_back(current);
    if (!result.empty() && !result.back().empty() && result.back().back() == '\r') {
        result.back().pop_back();
    }
    return result;
}

int getIndex(const std::vector<std::string>& header, const std::string& col) {
    auto it = std::find(header.begin(), header.end(), col);
    return (it != header.end()) ? static_cast<int>(std::distance(header.begin(), it)) : -1;
}

double parseTime(const std::string& t) {
    if (t.empty()) return 0.0;
    int h = 0, m = 0, s = 0;
    if (sscanf(t.c_str(), "%d:%d:%d", &h, &m, &s) == 3) {
        return h * 60.0 + m + (s / 60.0);
    }
    return 0.0;
}

double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
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
} // namespace

void GTFSParser::parse(const std::string& gtfsDir, Graph& graph, TransitMode defaultMode, const std::string& idPrefix) {
    std::cout << "Parsing GTFS from " << gtfsDir << "..." << std::endl;
    // 1. Parse routes.txt
    std::unordered_map<std::string, std::string> routes;
    {
        std::ifstream file(gtfsDir + "/routes.txt");
        if (file) {
            std::string line;
            std::getline(file, line);
            auto header = parseCSVLine(line);
            int idIdx = getIndex(header, "route_id");
            int shortIdx = getIndex(header, "route_short_name");
            int longIdx = getIndex(header, "route_long_name");

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                auto row = parseCSVLine(line);
                if (idIdx >= 0 && idIdx < row.size()) {
                    std::string id = row[idIdx];
                    std::string name = id;
                    if (shortIdx >= 0 && shortIdx < row.size() && !row[shortIdx].empty()) name = row[shortIdx];
                    else if (longIdx >= 0 && longIdx < row.size() && !row[longIdx].empty()) name = row[longIdx];
                    routes[id] = name;
                }
            }
        }
    }

    // 2. Parse stops.txt
    struct PrimaryStation {
        std::string id;
        double lat;
        double lon;
    };
    std::unordered_map<std::string, std::string> stopIdToPrimaryId;
    std::unordered_map<std::string, std::vector<PrimaryStation>> nameToPrimaryStations;
    {
        std::ifstream file(gtfsDir + "/stops.txt");
        if (file) {
            std::string line;
            std::getline(file, line);
            auto header = parseCSVLine(line);
            int idIdx = getIndex(header, "stop_id");
            int nameIdx = getIndex(header, "stop_name");
            int latIdx = getIndex(header, "stop_lat");
            int lonIdx = getIndex(header, "stop_lon");

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                auto row = parseCSVLine(line);
                if (idIdx >= 0 && idIdx < row.size()) {
                    std::string id = idPrefix + row[idIdx];
                    std::string name = (nameIdx >= 0 && nameIdx < row.size()) ? row[nameIdx] : id;
                    double lat = (latIdx >= 0 && latIdx < row.size() && !row[latIdx].empty()) ? std::stod(row[latIdx]) : 0.0;
                    double lon = (lonIdx >= 0 && lonIdx < row.size() && !row[lonIdx].empty()) ? std::stod(row[lonIdx]) : 0.0;
                    
                    std::string primaryId = "";
                    for (const auto& ps : nameToPrimaryStations[name]) {
                        if (haversineDistance(ps.lat, ps.lon, lat, lon) < 500.0) {
                            primaryId = ps.id;
                            break;
                        }
                    }

                    if (primaryId.empty()) {
                        primaryId = id;
                        nameToPrimaryStations[name].push_back({id, lat, lon});
                        graph.addStation(id, name, lat, lon);
                    }
                    stopIdToPrimaryId[id] = primaryId;
                }
            }
        }
    }

    // 3. Parse trips.txt
    std::unordered_map<std::string, std::string> trips; // trip_id -> route_id
    {
        std::ifstream file(gtfsDir + "/trips.txt");
        if (file) {
            std::string line;
            std::getline(file, line);
            auto header = parseCSVLine(line);
            int tripIdx = getIndex(header, "trip_id");
            int routeIdx = getIndex(header, "route_id");

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                auto row = parseCSVLine(line);
                if (tripIdx >= 0 && tripIdx < row.size() && routeIdx >= 0 && routeIdx < row.size()) {
                    trips[row[tripIdx]] = row[routeIdx];
                }
            }
        }
    }

    // 4. Parse stop_times.txt and aggregate edges deterministically
    struct EdgeData {
        double totalTime = 0.0;
        int count = 0;
    };
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>>> edgeStats;

    {
        std::ifstream file(gtfsDir + "/stop_times.txt");
        if (file) {
            std::string line;
            std::getline(file, line);
            auto header = parseCSVLine(line);
            int tripIdx = getIndex(header, "trip_id");
            int stopIdx = getIndex(header, "stop_id");
            int arrIdx = getIndex(header, "arrival_time");
            int depIdx = getIndex(header, "departure_time");

            std::string currentTrip = "";
            std::string prevStop = "";
            double prevDepTime = 0.0;

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                auto row = parseCSVLine(line);
                if (tripIdx < 0 || stopIdx < 0 || arrIdx < 0 || depIdx < 0 || 
                    tripIdx >= row.size() || stopIdx >= row.size() || arrIdx >= row.size() || depIdx >= row.size()) continue;

                std::string trip = row[tripIdx];
                std::string rawStop = idPrefix + row[stopIdx];
                std::string stop = stopIdToPrimaryId.count(rawStop) ? stopIdToPrimaryId[rawStop] : rawStop;
                double arrTime = parseTime(row[arrIdx]);
                double depTime = parseTime(row[depIdx]);

                if (trip == currentTrip && !prevStop.empty()) {
                    double travelTime = arrTime - prevDepTime;
                    if (travelTime < 0) travelTime += 24 * 60;

                    std::string route = trips[trip];
                    if (!route.empty()) {
                        edgeStats[route][prevStop][stop].totalTime += travelTime;
                        edgeStats[route][prevStop][stop].count += 1;
                    }
                } else {
                    currentTrip = trip;
                }
                prevStop = stop;
                prevDepTime = depTime;
            }
        }
    }

    // 5. Add aggregated edges to graph
    for (const auto& routePair : edgeStats) {
        std::string routeId = routePair.first;
        std::string routeName = routes.count(routeId) ? routes[routeId] : routeId;
        
        for (const auto& srcPair : routePair.second) {
            std::string src = srcPair.first;
            for (const auto& destPair : srcPair.second) {
                std::string dest = destPair.first;
                double avgTime = destPair.second.totalTime / destPair.second.count;
                
                if (avgTime < 0.5) avgTime = 0.5;
                if (avgTime > 30.0) continue; // Delete unrealistic edges

                graph.addConnection(src, dest, avgTime, defaultMode, routeName);
            }
        }
    }
}
