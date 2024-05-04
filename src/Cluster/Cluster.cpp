#include "Cluster.h"

void Cluster::AddNode(ClusterNode * node)
{
    this->_nodes[node->name] = node;
}

void Cluster::SetNodeLoad(std::string nodeId, int load)
{
    ClusterNode* node = nullptr;

    if (this->_nodes.count(nodeId) > 0)
    {
        node = this->_nodes[nodeId];
    }
    else {
        node = new ClusterNode(nodeId);
        this->AddNode(node);
    }

    node->load = load;
}

void Cluster::RemoveNode(std::string nodeId)
{
    auto it = _nodes.extract(nodeId);
    delete it.mapped();
}
