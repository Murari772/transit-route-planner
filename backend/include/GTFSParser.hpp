#pragma once
#include "Graph.hpp"
#include <string>

class GTFSParser {
public:
    // Parses a given GTFS directory and builds the provided graph.
    // defaultMode specifies whether to default to BUS or METRO if not explicit.
    static void parse(const std::string& gtfsDir, Graph& graph, TransitMode defaultMode, const std::string& idPrefix = "");
};
