#include "Topology.h"

vector<pair<double,double>> NodePositions = {
    {30, 90},   // 35
    {52, 70},   // 66
    {50, 66},   // 104
    {46, 50},   // 72
    {56, 58},   // 81
    {48, 12}    // 109
};

map<int, Node*> Nodes;

void initializeTopology()
{
    Nodes[35]  = new Node(35,  NodePositions[0].first, NodePositions[0].second, vector<int>{66,104});
    Nodes[66]  = new Node(66,  NodePositions[1].first, NodePositions[1].second, vector<int>{35,104});
    Nodes[104] = new Node(104, NodePositions[2].first, NodePositions[2].second, vector<int>{35,66,72,81});
    Nodes[72]  = new Node(72,  NodePositions[3].first, NodePositions[3].second, vector<int>{81,104,109});
    Nodes[81]  = new Node(81,  NodePositions[4].first, NodePositions[4].second, vector<int>{72,104,109});
    Nodes[109] = new Node(109, NodePositions[5].first, NodePositions[5].second, vector<int>{72,81});
}