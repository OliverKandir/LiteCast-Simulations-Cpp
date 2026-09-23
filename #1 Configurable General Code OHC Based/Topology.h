#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <vector>
#include <map>
#include <set>
#include <queue>
#include <cmath>

using namespace std;

class Data
{
public:
    int data;
    int ntx;
    int OHC;

    Data(int d, int n, int hc)
    {
        data = d;
        ntx = n;
        OHC = hc;
    }
};

class Node
{
public:
    int ID;
    double x;
    double y;

    int layer = -1;
    int HOP_COUNT;

    vector<int> NeighborIDs;
    map<int, double> NeighborDistances;

    set<int> DataAcquired;
    map<int, queue<Data*>> PacketsFor;
    map<int, set<int>> DataAcquiredFor;
    map<int, int> Transmitted;

    Node(int id, double xPos, double yPos, vector<int> neighborsIDs)
    {
        ID = id;
        x = xPos;
        y = yPos;
        NeighborIDs = neighborsIDs;
    }

    void initializeNeighborDistances(const map<int, Node*>& Nodes)
    {
        NeighborDistances.clear();

        for (int neighborID : NeighborIDs)
        {
            Node* neighbor = Nodes.at(neighborID);

            double dx = x - neighbor->x;
            double dy = y - neighbor->y;

            NeighborDistances[neighborID] = sqrt(dx * dx + dy * dy);
        }
    }
};

extern vector<pair<double,double>> NodePositions;
extern map<int, Node*> Nodes;

void initializeTopology();

#endif