#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <algorithm>
#include <iomanip>
#include <set>
using namespace std;

const int TIME_SLOTS = 6;
const int SUB_SLOTS = 5;
const int ntxOriginator = 10;
const int ntxForwarding = 1;
const int CENTER_NODE_ID = 104;


vector<pair<double,double>> NodePositions = {
    {30, 90},   //35
    {52, 70},   //66
    {50, 66},   //104
    {46, 50},   //72
    {56, 58},   //81
    {48, 12}    //109
};


//========================================================================================================
class Node{
public:
    int ID;
    double x; 
    double y;
    int layer = -1;
    vector<int> NeighborIDs;
    map<int, double> NeighborDistances;

    set<int> DataAcquired;
    map<int, queue<pair<int, int>>> PacketsFor;
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
vector<int> getContenders(vector<int> transmitters, map<int, vector<int>> &DataTransmitted, int subslot, int receiver){
    vector<int> contenders;
    for (int ID : transmitters){
        if(Nodes[receiver]->NeighborDistances.find(ID) == Nodes[receiver]->NeighborDistances.end()) continue;
        if(DataTransmitted[ID][subslot] != -1){
            contenders.push_back(ID);
        }
    }
    return contenders;
}
//========================================================================================================

vector<int> getWinnerNode(vector<int> contenders, map<int, vector<int>> &DataTransmitted, int receiver, int subslot)
{
    vector<int> winnerNodes;

    if (contenders.empty())
        return winnerNodes;

    //----------------------------------
    // Constructive interference
    //----------------------------------

    bool sameData = true;

    int firstData = DataTransmitted[contenders[0]][subslot];

    for (int contender : contenders){
        int data = DataTransmitted[contender][subslot];
        if (data != firstData){
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

void updateReceivers(int receiver, int receivedData, int subslot, int timeslot){

    //if already acquired then the received data acts as an acknowlegement for all the same queued data.
    if(Nodes[receiver]->DataAcquired.find(receivedData) != Nodes[receiver]->DataAcquired.end()){//if already acquired
        for (auto &[sub_slot, q] : Nodes[receiver]->PacketsFor){ //for every subslot check first element 
            if (!q.empty() and q.front().first == receivedData and Nodes[receiver]->Transmitted[receivedData] != 0){
                q.pop(); 
            }
        }
    }

    set<int> acquiredSet = Nodes[receiver]->DataAcquiredFor[subslot];
    bool acquired_for_the_subslot = (acquiredSet.find(receivedData) != acquiredSet.end());

    if(!acquired_for_the_subslot){ //not acknowleged for the subslot
        Nodes[receiver]->PacketsFor[subslot].push(make_pair(receivedData, ntxForwarding));
        Nodes[receiver]->DataAcquired.insert(receivedData);
        Nodes[receiver]->DataAcquiredFor[subslot].insert(receivedData);
    }
}

//========================================================================================================

void displayAcquiredData(int slot)
{
    cout << "\n================ END OF SLOT " << slot  << " =================\n\n";
    for (auto &[id, node] : Nodes)
    {
        cout << left << setw(8) << ("Node " + to_string(id)) << " : ";
        for (auto data : node->DataAcquired){
            cout << setw(5) << data;
        }
        cout << '\n';
    }
    cout << "\n================================================\n";
}

void displayFrontoftheQueue(int slot){
    cout << "\n================ QUEUE STATUS AT SLOT " << slot << " =================\n\n";
    for (auto &[id, node] : Nodes){
        cout << "Node " << id << ":\n";
        bool hasPackets = false;
        for(int i = 0; i < SUB_SLOTS; i++){
            queue<pair<int, int>> q = Nodes[id]->PacketsFor[i];
            if(!q.empty()){
                hasPackets = true;
                cout << "  Subslot " << i << ": ";
                queue<pair<int, int>> temp = q;
                while(!temp.empty()){
                    cout << "(" << temp.front().first << "," << temp.front().second << ") ";
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

//========================================================================================================

bool is_a_transmitter(int receiver, map<int, vector<int>> &DataTransmitted, int subslot){
    if(DataTransmitted[receiver][subslot] != -1) return true;
    return false;
}

//========================================================================================================
int main(){

    Nodes[35]  = new Node(35,  NodePositions[0].first, NodePositions[0].second, vector<int>{66,104});
    Nodes[66]  = new Node(66,  NodePositions[1].first, NodePositions[1].second, vector<int>{35,104});
    Nodes[104] = new Node(104, NodePositions[2].first, NodePositions[2].second, vector<int>{35,66,72,81});
    Nodes[72]  = new Node(72,  NodePositions[3].first, NodePositions[3].second, vector<int>{81,104,109});
    Nodes[81]  = new Node(81,  NodePositions[4].first, NodePositions[4].second, vector<int>{72,104,109});
    Nodes[109] = new Node(109, NodePositions[5].first, NodePositions[5].second, vector<int>{72,81});
    
    for (auto& [id, node] : Nodes)
    {
        node->initializeNeighborDistances(Nodes);
        node->DataAcquired.insert(node->ID);
        node->PacketsFor[node->ID%SUB_SLOTS].push(make_pair(node->ID, ntxOriginator));
        node->DataAcquiredFor[node->ID%SUB_SLOTS].insert(node->ID);
    }

    Node* CENTER_NODE = Nodes[CENTER_NODE_ID];
    queue<Node*> Queue;
    CENTER_NODE->layer = 0;
    Queue.push(CENTER_NODE);
    
    while(!Queue.empty()){
        Node* currentnode = Queue.front();
        Queue.pop();
        int currentlayer = currentnode->layer;
        int neighborlayer = (currentlayer+1) % 2;
        for(auto id : currentnode->NeighborIDs){
            Node* neighbornode = Nodes[id];
            if(neighbornode->layer == -1){
                neighbornode->layer = neighborlayer;
                Queue.push(neighbornode);
            }
        }
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


    vector<int> nodeids;

    for(auto [id, node] : Nodes){
        nodeids.push_back(id);
    }

    
    for(int timeslot = 1; timeslot <= TIME_SLOTS; timeslot++ ){
        
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

        if(timeslot == 1) displayFrontoftheQueue(timeslot);

        map<int, vector<int>> DataTransmitted;
        for(int id : nodeids){
            DataTransmitted[id] = vector<int>(SUB_SLOTS, -1);
        }

        for(int subslot = 0; subslot < SUB_SLOTS;  subslot++){
            /* cout << "-------------------------------------" << endl;
            cout << "Sublslot: " << subslot << endl;
            cout << "-------------------------------------" << endl; */
            
            for(int transmitter: transmitters){
                auto it = Nodes[transmitter]->PacketsFor.find(subslot);
                if (it != Nodes[transmitter]->PacketsFor.end() && !it->second.empty()){
                    
                    DataTransmitted[transmitter][subslot] = it->second.front().first;
                    Nodes[transmitter]->Transmitted[it->second.front().first] += 1;
                    Nodes[transmitter]->PacketsFor[subslot].front().second -= 1;

                    int timesTransmitted = Nodes[transmitter]->PacketsFor[subslot].front().second;
                    if(timesTransmitted == 0){
                        Nodes[transmitter]->PacketsFor[subslot].pop();
                    }
                }
            }

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
                    updateReceivers( receiver, DataTransmitted[winners[0]][subslot] ,subslot, timeslot);        
                }
            }
        }
        displayAcquiredData(timeslot);
        displayFrontoftheQueue(timeslot);
    }
};

