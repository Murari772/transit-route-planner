#pragma once
#include "Graph.hpp"
#include <string>

class DataLoader {
public:
    static void loadMetroNetwork(const std::string& filepath, Graph& graph);
};
