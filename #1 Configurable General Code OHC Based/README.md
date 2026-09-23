#1 Topology related details must be configured in Topology.cpp

The NodePositions structure basically stores x and y coordinates. So for example you have 3 nodes then this structure would have 3 x and y coordinates.

However These coordinates are not actually mapped to any particular node.

The mapping of Cooridinates to any particular node happens in Nodes Initialization via initializeTopology() method.
 
It is essential to specify the Node ID, its Coordinates and List of Neighbors (i.e List of Nodes in its current Wireless Range). while Initializing the Nodes.

so for example you have 3 nodes then you must have 3 node initializations inside the initializeTopology() method.

#2 Configuratios specific to the working of Protocol is configurable in Configuration.cpp

Details are as follows

const int TIME_SLOT: determines how many time slots the simulation runs

const int SUB_SLOTS: determines how many subslots are there in one TIME_SLOT it basically tell the TIME_SLOT size or the Slot Size from the paper.

const int ntxOriginator: ntx associated with the Data is Number of Times the particular data has to be transmitted. nxtOriginator specifically is set by the owner
of the data. Node that is responsible for producing that data.

const int ntxForwarding: this ntx is set for the data by the intermediate nodes that are not the owener of the data but receives it.

const int CENTER_NODE_ID: is the ID of a particular node that functions as the originator of the Protocol.

const int LC: sets the protocol to capture specific to a locality, So for example LC is set to 3 then the protocol captures data upto LC-3 of 3 hop neighbors.





