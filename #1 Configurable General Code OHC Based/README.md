#1 Topology-related details must be configured in `Topology.cpp`.

The `NodePositions` structure stores the x and y coordinates of the nodes. For example, if there are 3 nodes, this structure will contain the x and y coordinates of all 3 nodes.

However, these coordinates are not directly mapped to any particular node.

The mapping of coordinates to a particular node happens during node initialization through the `initializeTopology()` method.

It is essential to specify the **Node ID**, its **coordinates**, and its **list of neighbors** (i.e., the list of nodes within its current wireless range) while initializing the nodes.

For example, if there are 3 nodes, there must be 3 node initializations inside the `initializeTopology()` method.

#2 Protocol-specific configurations can be configured in `Configuration.cpp`.

## Configuration Details

### `const int TIME_SLOT`

Determines how many time slots the simulation runs for.

### `const int SUB_SLOTS`

Determines how many subslots are present in one `TIME_SLOT`. It essentially specifies the `TIME_SLOT` size, or the **Slot Size** referred to in the paper.

### `const int ntxOriginator`

The `ntx` associated with the data specifies the number of times the particular data has to be transmitted.

`ntxOriginator` is specifically set by the owner of the data, i.e., the node responsible for producing that data.

### `const int ntxForwarding`

This `ntx` value is set for the data by intermediate nodes that are not the owner of the data but receive and forward it.

### `const int CENTER_NODE_ID`

Specifies the ID of the particular node that functions as the originator of the protocol.

### `const int LC`

Specifies the locality for which the protocol captures data.

For example, if `LC` is set to `3`, the protocol captures data up to **LC-3**, i.e., from nodes within 3 hops of the center node.
