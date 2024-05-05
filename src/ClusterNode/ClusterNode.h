#pragma once

#include <string>
#include <optional>

class ClusterNode {
private:
    
public:
    ClusterNode();
    ClusterNode(std::string name);
    ClusterNode(const ClusterNode& other);
    std::string name;
    std::optional<int> load;
};
