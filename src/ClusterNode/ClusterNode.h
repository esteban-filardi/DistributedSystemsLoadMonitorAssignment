#pragma once

#include <string>
#include <optional>

class ClusterNode {
private:
    
public:
    ClusterNode(std::string name);

    std::string name;
    std::optional<int> load;
};
