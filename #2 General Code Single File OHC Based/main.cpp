#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <algorithm>
#include <iomanip>
#include <set>
using namespace std;

const int TIME_SLOT = 6;
const int SUB_SLOTS = 16;
const int ntxOriginator = 10;
const int ntxForwarding = 1;
const int CENTER_NODE_ID = 17;
int LC = 1;

vector<pair<double, double>> NodePositions = {
    {118.08, 1133.28}, {27.36, 1052.64},
    {72.00, 964.80}, {200.16, 1072.80},
    {190.08, 963.36}, {341.28, 1101.60},
    {299.52, 995.04}, {187.20, 885.60},
    {60.48, 823.68}, {452.16, 1069.92},
    {404.64, 946.08}, {311.04, 869.76},
    {182.88, 789.12}, {498.24, 1006.56},
    {311.04, 773.28}, {629.28, 1018.08},
    {689.76, 1006.56}, {745.92, 979.20},
    {871.20, 872.64}, {940.32, 1015.20},
    {959.04, 924.48}, {951.84, 823.68},
    {946.08, 731.52}, {938.88, 648.00},
    {1041.12, 950.40}, {1032.48, 848.16},
    {1045.44, 773.28}, {1031.04, 686.88},
    {1094.40, 1064.16}, {1108.80, 969.12},
    {1111.68, 878.40}, {1097.28, 786.24},
    {1092.96, 689.76}, {1103.04, 594.72},
    {1164.96, 1015.20}, {1182.24, 914.40},
    {1175.04, 825.12}, {1172.16, 747.36},
    {1175.04, 663.84}, {1241.28, 597.60},
    {1254.24, 982.08}, {1265.76, 872.64},
    {1261.44, 780.48}, {1255.68, 708.48},
    {1320.48, 830.88}, {1199.52, 1104.48},
    {1324.80, 1016.64}
};

class Data{
public:
    int data;
    int ntx;
    int OHC;
    Data(int d, int n, int hc){
        data = d;
        ntx = n;
        OHC = hc;
    }
};

//========================================================================================================
class Node{
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

map<int, Node*> Nodes;

//========================================================================================================

pair<vector<int>, vector<int>> get_Transmitters_Receivers(vector<int> nodes,int timeslot){
    int odd = 1;
    int even = 0;
    vector<int> transmitters;
    vector<int> receivers;
    if(timeslot%2 == odd){
        for(int ID: nodes){
            if (Nodes[ID]->layer == even){
                receivers.push_back(ID);
            }else{
                transmitters.push_back(ID);
            }   
        }
    }else{
        for(int ID: nodes){
            if (Nodes[ID]->layer == odd){
                receivers.push_back(ID);
            }else{
                transmitters.push_back(ID);
            }
        }
    }

    return make_pair(transmitters, receivers);
}

//========================================================================================================

//A node becomes a contender if it has at least one packet queued for that subslot.
vector<int> getContenders(vector<int> transmitters, map<int, vector<Data*>> &DataTransmitted, int subslot, int receiver){
    vector<int> contenders;
    for (int ID : transmitters){
        if(Nodes[receiver]->NeighborDistances.find(ID) == Nodes[receiver]->NeighborDistances.end()) continue;
        if(DataTransmitted[ID][subslot] != nullptr){
            contenders.push_back(ID);
        }
    }
    return contenders;
}
//========================================================================================================

vector<int> getWinnerNode(vector<int> contenders, map<int, vector<Data*>> &DataTransmitted, int receiver, int subslot)
{
    vector<int> winnerNodes;

    if (contenders.empty())
        return winnerNodes;

    //----------------------------------
    // Constructive interference
    //----------------------------------

    bool sameData = true;

    Data* firstData = DataTransmitted[contenders[0]][subslot];

    for (int contender : contenders){
        Data* Data = DataTransmitted[contender][subslot];
        if (Data->data != firstData->data or Data->OHC != firstData->OHC){
            sameData = false;
            break;
        }
    }

    if (sameData){
        // All contenders transmit the same packet
        return contenders;
    }

    //----------------------------------
    // Capture effect
    //----------------------------------

    double sumOfPowers = 0.0;
    double maxSignal = -1.0;
    int strongestNode = -1;

    for (int contender : contenders)
    {
        double receivePower =
            1.0 / pow(Nodes[contender]->NeighborDistances[receiver], 3.0);

        if (receivePower > maxSignal)
        {
            maxSignal = receivePower;
            strongestNode = contender;
        }

        sumOfPowers += receivePower;
    }

    double interference = sumOfPowers - maxSignal;

    if (maxSignal >= 2.0 * interference)
    {
        winnerNodes.push_back(strongestNode);
    }

    return winnerNodes;
}

//========================================================================================================

void updateReceivers(int receiver, Data* receivedData, int subslot){
    
    if(Nodes[receiver]->DataAcquired.find(receivedData->data) != Nodes[receiver]->DataAcquired.end()){//if already acquired
        for (auto &[sub_slot, q] : Nodes[receiver]->PacketsFor){ //for every subslot check first element 
            if (!q.empty() and q.front()->data == receivedData->data and Nodes[receiver]->Transmitted[receivedData->data] != 0){
                q.pop();
            }
        }
    }

    int x = receivedData->OHC;
    int y = Nodes[receiver]->HOP_COUNT;
    if(abs(x-y) > LC) return;


    set<int> acquiredSet = Nodes[receiver]->DataAcquiredFor[subslot];
    bool acquired_for_the_subslot = (acquiredSet.find(receivedData->data) != acquiredSet.end());

    if(!acquired_for_the_subslot){ //not acknowleged for the subslot
        Nodes[receiver]->PacketsFor[subslot].push(new Data(receivedData->data, ntxForwarding, receivedData->OHC));
        Nodes[receiver]->DataAcquired.insert(receivedData->data);
        Nodes[receiver]->DataAcquiredFor[subslot].insert(receivedData->data);
    }
}

//========================================================================================================

void displayAcquiredData(int slot, map<int, set<int>> khopneihbors)
{
    cout << "\n================ END OF SLOT " << slot << " =================\n\n";

    for (auto &[id, node] : Nodes)
    {
        cout << left << setw(8) << ("Node " + to_string(id)) << " : ";

        for (int data : node->DataAcquired)
        {
            cout << setw(5) << data;
        }

        int acquired = 0;

        for(int neighbor : khopneihbors[node->ID]){
            if(node->DataAcquired.count(neighbor)) acquired++;
        }


        double percentage = 100.0 * acquired / (khopneihbors[node->ID].size());

        cout << " : "<< fixed << setprecision(1) << setw(5) << percentage << "%\n";

    }

    cout << "\n================================================\n";
}

//========================================================================================================

void displayFrontoftheQueue(int slot, int ID){

    cout << "\n================ QUEUE STATUS AT SLOT " << slot << " =================\n\n";

    if(Nodes.find(ID) == Nodes.end()){
        cout << "Invalid ID is passed to displayFrontoftheQueue()" <<endl << endl;
    }

    vector<int> IDs = Nodes[ID]->NeighborIDs;
    IDs.push_back(ID);
    for (auto id: IDs){
        cout << "Node " << id << ":\n";
        bool hasPackets = false;
        for(int i = 0; i < SUB_SLOTS; i++){
            queue<Data*> q = Nodes[id]->PacketsFor[i];
            if(!q.empty()){
                hasPackets = true;
                cout << "  Subslot " << i << ": ";
                queue<Data*> temp = q;
                while(!temp.empty()){
                    cout << "(" << temp.front()->data << "," << temp.front()->ntx << "," << temp.front()->OHC << ") ";
                    temp.pop();
                }
                cout << "\n";
            }
        }
        if(!hasPackets){
            cout << "  All queues empty\n";
        }
        cout << "\n";
    }
    cout << "================================================\n";
}

void display_NodeLayer_and_HopCount(){    
    for(auto [id, node]: Nodes){
        cout << "Node " << id << " has parity: " << node->layer <<" and has OHC: " << node->HOP_COUNT << endl;
    }
}


//========================================================================================================

bool is_a_transmitter(int receiver, map<int, vector<Data*>> &DataTransmitted, int subslot){
    if(DataTransmitted[receiver][subslot] != nullptr) return true;
    return false;
}

//========================================================================================================

set<int> Get_K_HOP_Neighbors(int id, int ttl){
    set<int> visited;
    set<int> neighborswithinkhop;
    
    queue<pair<int,int>> Queue;
    Queue.push(make_pair(id, 0));
    visited.insert(id);

    while(!Queue.empty()){
        auto [id, level] = Queue.front();
        Queue.pop();
        
        vector<int> neighbors = Nodes[id]->NeighborIDs;
        for(auto neighbor: neighbors){
            if(!visited.count(neighbor) and level < ttl){
                visited.insert(neighbor);
                neighborswithinkhop.insert(neighbor);
                Queue.push(make_pair(neighbor, level+1));
            }
        }
    }

    return neighborswithinkhop;
}


//========================================================================================================
int main(){

    Nodes[1] = new Node(1, NodePositions[0].first, NodePositions[0].second, {2, 3, 4});
    Nodes[2] = new Node(2, NodePositions[1].first, NodePositions[1].second, {1, 3, 4, 5});
    Nodes[3] = new Node(3, NodePositions[2].first, NodePositions[2].second, {1, 2, 4, 5});
    Nodes[4] = new Node(4, NodePositions[3].first, NodePositions[3].second, {1, 2, 3, 5});
    Nodes[5] = new Node(5, NodePositions[4].first, NodePositions[4].second, {2, 3, 4, 7, 8, 11});
    Nodes[6] = new Node(6, NodePositions[5].first, NodePositions[5].second, {7, 10});
    Nodes[7] = new Node(7, NodePositions[6].first, NodePositions[6].second, {5, 6, 9, 10, 11, 14});
    Nodes[8] = new Node(8, NodePositions[7].first, NodePositions[7].second, {5, 9, 10, 12, 13, 15});
    Nodes[9] = new Node(9, NodePositions[8].first, NodePositions[8].second, {7, 8, 12, 13});
    Nodes[10] = new Node(10, NodePositions[9].first, NodePositions[9].second, {6, 7, 8, 14});
    Nodes[11] = new Node(11, NodePositions[10].first, NodePositions[10].second, {5, 7, 12, 13, 14});
    Nodes[12] = new Node(12, NodePositions[11].first, NodePositions[11].second, {8, 9, 11, 13, 14, 15});
    Nodes[13] = new Node(13, NodePositions[12].first, NodePositions[12].second, {8, 9, 11, 12, 15});
    Nodes[14] = new Node(14, NodePositions[13].first, NodePositions[13].second, {7, 10, 11, 12, 15, 16});
    Nodes[15] = new Node(15, NodePositions[14].first, NodePositions[14].second, {8, 12, 13, 14});
    Nodes[16] = new Node(16, NodePositions[15].first, NodePositions[15].second, {14, 17});
    Nodes[17] = new Node(17, NodePositions[16].first, NodePositions[16].second, {16, 18});
    Nodes[18] = new Node(18, NodePositions[17].first, NodePositions[17].second, {17, 19});
    Nodes[19] = new Node(19, NodePositions[18].first, NodePositions[18].second, {18, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 32});
    Nodes[20] = new Node(20, NodePositions[19].first, NodePositions[19].second, {19, 21, 25, 26, 27, 29, 30, 31, 33, 36, 37});
    Nodes[21] = new Node(21, NodePositions[20].first, NodePositions[20].second, {19, 20, 22, 25, 26, 27, 28, 29, 30, 31, 32});
    Nodes[22] = new Node(22, NodePositions[21].first, NodePositions[21].second, {19, 21, 23, 24, 25, 26, 27, 28, 30, 31, 32, 33, 35});
    Nodes[23] = new Node(23, NodePositions[22].first, NodePositions[22].second, {19, 22, 24, 25, 26, 27, 28, 30, 31, 32, 33, 38});
    Nodes[24] = new Node(24, NodePositions[23].first, NodePositions[23].second, {19, 22, 23, 33, 44});
    Nodes[25] = new Node(25, NodePositions[24].first, NodePositions[24].second, {19, 20, 21, 22, 23, 26, 27, 28, 29, 30, 31, 35, 36});
    Nodes[26] = new Node(26, NodePositions[25].first, NodePositions[25].second, {19, 20, 21, 22, 23, 25, 27, 28, 29, 30, 31, 32, 36});
    Nodes[27] = new Node(27, NodePositions[26].first, NodePositions[26].second, {19, 20, 21, 22, 23, 25, 26, 28, 30, 31, 32, 33, 37});
    Nodes[28] = new Node(28, NodePositions[27].first, NodePositions[27].second, {19, 21, 22, 23, 25, 26, 27, 31, 32, 33, 34, 38, 39});
    Nodes[29] = new Node(29, NodePositions[28].first, NodePositions[28].second, {19, 20, 21, 25, 26, 30, 31, 35, 36, 37, 41, 42, 46});
    Nodes[30] = new Node(30, NodePositions[29].first, NodePositions[29].second, {20, 21, 22, 23, 25, 26, 27, 29, 31, 35, 36, 37, 41});
    Nodes[31] = new Node(31, NodePositions[30].first, NodePositions[30].second, {20, 21, 22, 23, 25, 26, 27, 28, 29, 30, 32, 35, 36, 37, 38, 42});
    Nodes[32] = new Node(32, NodePositions[31].first, NodePositions[31].second, {19, 21, 22, 23, 26, 27, 28, 31, 33, 36, 37, 38, 39, 43});
    Nodes[33] = new Node(33, NodePositions[32].first, NodePositions[32].second, {20, 22, 23, 24, 27, 28, 32, 37, 38, 39, 44});
    Nodes[34] = new Node(34, NodePositions[33].first, NodePositions[33].second, {28, 39});
    Nodes[35] = new Node(35, NodePositions[34].first, NodePositions[34].second, {22, 25, 29, 30, 31, 36, 37, 41, 42, 43});
    Nodes[36] = new Node(36, NodePositions[35].first, NodePositions[35].second, {20, 25, 26, 29, 30, 31, 32, 35, 37, 41, 42, 43});
    Nodes[37] = new Node(37, NodePositions[36].first, NodePositions[36].second, {20, 27, 29, 30, 31, 32, 33, 35, 36, 41, 42, 43, 44, 45});
    Nodes[38] = new Node(38, NodePositions[37].first, NodePositions[37].second, {23, 28, 31, 32, 33, 39, 42, 43, 44, 45});
    Nodes[39] = new Node(39, NodePositions[38].first, NodePositions[38].second, {28, 32, 33, 34, 38, 40, 42, 43, 44, 45});
    Nodes[40] = new Node(40, NodePositions[39].first, NodePositions[39].second, {39});
    Nodes[41] = new Node(41, NodePositions[40].first, NodePositions[40].second, {29, 30, 35, 36, 37, 42, 43, 45, 46, 47});
    Nodes[42] = new Node(42, NodePositions[41].first, NodePositions[41].second, {29, 31, 35, 36, 37, 38, 39, 41, 43, 44, 45});
    Nodes[43] = new Node(43, NodePositions[42].first, NodePositions[42].second, {32, 35, 36, 37, 38, 39, 41, 42, 44, 45});
    Nodes[44] = new Node(44, NodePositions[43].first, NodePositions[43].second, {24, 33, 37, 38, 39, 42, 43, 45});
    Nodes[45] = new Node(45, NodePositions[44].first, NodePositions[44].second, {37, 38, 39, 41, 42, 43, 44});
    Nodes[46] = new Node(46, NodePositions[45].first, NodePositions[45].second, {29, 41, 47});
    Nodes[47] = new Node(47, NodePositions[46].first, NodePositions[46].second, {41, 46});

    if(Nodes.find(CENTER_NODE_ID) == Nodes.end()){
        cout << "Center Node ID is not configured properly" << endl << endl;
    }

    Node* CENTER_NODE = Nodes[CENTER_NODE_ID];
    CENTER_NODE->HOP_COUNT = 0;
    CENTER_NODE->layer = 0;

    queue<Node*> Queue;
    Queue.push(CENTER_NODE);
    
    while(!Queue.empty()){
        Node* currentnode = Queue.front();
        Queue.pop();
        
        int currentlayer = currentnode->layer;
        int neighborlayer = (currentlayer+1) % 2;
        int neighborHOP_COUNT = currentnode->HOP_COUNT + 1; 

        for(auto id : currentnode->NeighborIDs){
            Node* neighbornode = Nodes[id];
            if(neighbornode->layer == -1){
                neighbornode->layer = neighborlayer;
                neighbornode->HOP_COUNT = neighborHOP_COUNT;
                Queue.push(neighbornode);
            }
        }
    }

    display_NodeLayer_and_HopCount();

    for (auto& [id, node] : Nodes)
    {
        node->initializeNeighborDistances(Nodes);
        node->DataAcquired.insert(node->ID);
        node->PacketsFor[node->ID%SUB_SLOTS].push(new Data(node->ID, ntxOriginator, node->HOP_COUNT));
        node->DataAcquiredFor[node->ID%SUB_SLOTS].insert(node->ID);
    }


    cout << "==============================================================" << endl;
    cout << "all even parity nodes" << endl;
    for(auto [id, node] : Nodes){
        if(node->layer == 0) cout << node->ID << " ";
    }cout << endl;
    cout << "all odd parity nodes" << endl;
    for(auto [id, node] : Nodes){
        if(node->layer != 0) cout << node->ID << " ";
    }cout << endl;

    cout << "==============================================================" << endl;

    map<int, set<int>> K_HOP_Neighbors;

    for (auto &[id, node] : Nodes){
       K_HOP_Neighbors[id] = Get_K_HOP_Neighbors(id, LC);
    }

    vector<int> nodeids;

    for(auto [id, node] : Nodes){
        nodeids.push_back(id);
    }
    
    
    for(int timeslot = 1; timeslot <= TIME_SLOT; timeslot++ ){
        
        pair<vector<int>, vector<int>> group = get_Transmitters_Receivers(nodeids, timeslot);
        vector<int> transmitters = group.first;
        vector<int> receivers = group.second;
        
        cout << endl << "Slot: " <<timeslot<< endl;
        cout << "transmitters: ";
        for(auto i: transmitters){
            cout << i << " ";
        }cout<< endl << "receivers : ";
        for(auto i: receivers){
            cout << i << " ";
        }cout << endl << endl;


        //displayFrontoftheQueue(timeslot, 38);

        map<int, vector<Data*>> DataTransmitted;
        for(int id : nodeids){
            DataTransmitted[id] = vector<Data*>(SUB_SLOTS, nullptr);
        }

        for(int subslot = 0; subslot < SUB_SLOTS;  subslot++){
            /* cout << "-------------------------------------" << endl;
            cout << "Sublslot: " << subslot << endl;
            cout << "-------------------------------------" << endl; */
            
            for(int transmitter: transmitters){
                auto it = Nodes[transmitter]->PacketsFor.find(subslot);
                if (it != Nodes[transmitter]->PacketsFor.end() && !it->second.empty()){
                    
                    DataTransmitted[transmitter][subslot] = it->second.front();
                    Nodes[transmitter]->Transmitted[it->second.front()->data] += 1;
                    Nodes[transmitter]->PacketsFor[subslot].front()->ntx -= 1;

                    int timesTransmitted = Nodes[transmitter]->PacketsFor[subslot].front()->ntx;
                    if(timesTransmitted == 0){
                        Nodes[transmitter]->PacketsFor[subslot].pop();
                    }
                }
            }

            set<int> winnersSet;
            for(int receiver : nodeids){

                if(is_a_transmitter(receiver, DataTransmitted, subslot)) continue;

                vector<int> contenders = getContenders(transmitters, DataTransmitted, subslot, receiver);
                /* cout << "Contenders for receiver: " << receiver << ": ";
                for(int c : contenders) cout << c << " ";
                cout << endl; */

                vector<int> winners = getWinnerNode(contenders, DataTransmitted, receiver, subslot);
                /* cout << endl << "Winner for receiver: " << receiver << " is/are: ";
                for(int i : winners)cout << i << " ";
                cout << endl; */
            
                if(!winners.empty()){
                    updateReceivers( receiver, DataTransmitted[winners[0]][subslot] ,subslot);        
                }
            }
        }
        displayAcquiredData(timeslot, K_HOP_Neighbors);
        //displayFrontoftheQueue(timeslot, 38);
    }

    
    cout << "Neighbors: ";
    for(auto s: K_HOP_Neighbors[29]){
        cout << s << " ";
    }
    cout << endl;
      
    
    double averageCumulative = 0.0;

    for (auto &[id, node] : Nodes){
        int totalOpposite = K_HOP_Neighbors[id].size();
        int acquired = 0;
        for (int neighbor : K_HOP_Neighbors[id])
        {
            if (node->DataAcquired.count(neighbor))
                acquired++;
        }
        double fraction = 0.0;

        if (totalOpposite > 0)fraction = static_cast<double>(acquired) / totalOpposite;

        averageCumulative += fraction;
    }
    averageCumulative /= Nodes.size();
    cout << "\nAverage cumulative acquisition = "<< fixed << setprecision(2) << averageCumulative * 100.0 << "%\n" << endl;
};

