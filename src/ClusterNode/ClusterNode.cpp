#include "ClusterNode.h"

ClusterNode::ClusterNode() {};
ClusterNode::ClusterNode(std::string name) : name(name) {}

ClusterNode::ClusterNode(const ClusterNode& other) : name(other.name), load(other.load), loadTimestamp(other.loadTimestamp) {};
