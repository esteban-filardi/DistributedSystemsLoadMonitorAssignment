#pragma once

#include <string>
#include <optional>
#include <map>
#include <mutex>
#include "../ClusterNode/ClusterNode.h"

class Cluster {
private:
    std::map<std::string, ClusterNode *> _nodes;
    std::mutex _mutex;
public:
    /// <summary>
    /// Gets a snapshot of the internal node map to ensure thread safety when presenting the nodes.
    /// </summary>
    /// <returns></returns>
    std::map<std::string, ClusterNode> GetClusterNodesSnapshot();    
    void AddNode(ClusterNode* node);
    void SetNodeLoad(std::string nodeId, int load);
    void RemoveNode(std::string nodeId);
};

