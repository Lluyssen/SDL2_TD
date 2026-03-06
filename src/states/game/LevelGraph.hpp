#pragma once
#include <vector>
#include "LevelNode.hpp"

struct LevelEdge
{
    int from;
    int to;
};

class LevelGraph
{
private:

    std::vector<LevelNode> _nodes;
    std::vector<LevelEdge> _edges;

public:

    void addNode(const LevelNode& node)
    {
        _nodes.push_back(node);
    }

    void addEdge(int from,int to)
    {
        _edges.push_back({from,to});
    }

    std::vector<LevelNode>& nodes()
    {
        return _nodes;
    }

    const std::vector<LevelEdge>& edges() const
    {
        return _edges;
    }
};