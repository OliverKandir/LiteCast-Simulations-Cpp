#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <algorithm>
#include <iomanip>
#include <set>

#include "Topology.h"
#include "Configuration.h"

using namespace std;

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

    initializeTopology();

    if(Nodes.find(CENTER_NODE_ID) == Nodes.end()){
        cout << "Center Node is not configured properly" << endl << endl;
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
        
        displayFrontoftheQueue(timeslot, 35);

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

#include "Topology.cpp"
#include "Configuration.cpp"