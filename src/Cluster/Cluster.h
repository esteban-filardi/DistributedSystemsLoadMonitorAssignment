#pragma once

#include <string>
#include <optional>
#include <map>
#include "../ClusterNode/ClusterNode.h"

class Cluster {
private:
    std::map<std::string, ClusterNode *> _nodes;
public:
    void AddNode(ClusterNode* node);
    void SetNodeLoad(std::string nodeId, int load);
    void RemoveNode(std::string nodeId);
};

