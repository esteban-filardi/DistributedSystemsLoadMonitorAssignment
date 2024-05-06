#include "Cluster.h"

void Cluster::AddNode(ClusterNode * node)
{
    std::lock_guard<std::mutex> lock(_mutex);

    this->_nodes[node->name] = node;
}

void Cluster::SetNodeLoad(std::string nodeId, float load, std::string loadTimestamp)
{
    std::lock_guard<std::mutex> lock(_mutex);

    ClusterNode* node = nullptr;


    if (this->_nodes.count(nodeId) > 0)
    {
        node = this->_nodes[nodeId];
    }
    else {
        node = new ClusterNode(nodeId);
        _nodes[nodeId] = node;
    }

    node->load = load;
    node->loadTimestamp = loadTimestamp;
}

void Cluster::RemoveNode(std::string nodeId)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_nodes.count(nodeId) == 0)
    {
        return;
    }


    auto it = _nodes.extract(nodeId);
    delete it.mapped();
}

std::map<std::string, ClusterNode> Cluster::GetClusterNodesSnapshot()
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto mapSnapshot = std::map<std::string, ClusterNode>();

    for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        ClusterNode copiedNode(*it->second);
        mapSnapshot[copiedNode.name] = copiedNode;
    }

    return mapSnapshot;
}
