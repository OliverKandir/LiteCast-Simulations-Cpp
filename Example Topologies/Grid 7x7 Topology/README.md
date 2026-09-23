
# 7x7 Grid Topology

This folder contains the **7x7 grid topology** used in the LiteCast simulations.

## Topology Configuration

- **Grid Size:** 7x7
- **Number of Nodes:** 49
- **Center Node ID:** 1

## Node Positions

The following C++ `NodePositions` vector defines the `(x, y)` coordinates of the 49 nodes:

```cpp
vector<pair<double, double>> NodePositions = {
    {500,500}, // 1
    {600,500}, // 2
    {600,400}, // 3
    {500,400}, // 4
    {400,400}, // 5
    {400,500}, // 6
    {400,600}, // 7
    {500,600}, // 8
    {600,600}, // 9
    {700,600}, // 10
    {700,500}, // 11
    {700,400}, // 12
    {700,300}, // 13
    {600,300}, // 14
    {500,300}, // 15
    {400,300}, // 16
    {300,300}, // 17
    {300,400}, // 18
    {300,500}, // 19
    {300,600}, // 20
    {300,700}, // 21
    {400,700}, // 22
    {500,700}, // 23
    {600,700}, // 24
    {700,700}, // 25
    {800,700}, // 26
    {800,600}, // 27
    {800,500}, // 28
    {800,400}, // 29
    {800,300}, // 30
    {800,200}, // 31
    {700,200}, // 32
    {600,200}, // 33
    {500,200}, // 34
    {400,200}, // 35
    {300,200}, // 36
    {200,200}, // 37
    {200,300}, // 38
    {200,400}, // 39
    {200,500}, // 40
    {200,600}, // 41
    {200,700}, // 42
    {200,800}, // 43
    {300,800}, // 44
    {400,800}, // 45
    {500,800}, // 46
    {600,800}, // 47
    {700,800}, // 48
    {800,800}  // 49
};
```

## Nodes Initialization

Each node is initialized with its node ID, position, and list of neighboring nodes:

```cpp
Nodes[1] = new Node(1, NodePositions[0].first, NodePositions[0].second, {4, 8, 6, 2});
Nodes[2] = new Node(2, NodePositions[1].first, NodePositions[1].second, {3, 9, 1, 11});
Nodes[3] = new Node(3, NodePositions[2].first, NodePositions[2].second, {14, 2, 4, 12});
Nodes[4] = new Node(4, NodePositions[3].first, NodePositions[3].second, {15, 1, 5, 3});
Nodes[5] = new Node(5, NodePositions[4].first, NodePositions[4].second, {16, 6, 18, 4});
Nodes[6] = new Node(6, NodePositions[5].first, NodePositions[5].second, {5, 7, 19, 1});
Nodes[7] = new Node(7, NodePositions[6].first, NodePositions[6].second, {6, 22, 20, 8});
Nodes[8] = new Node(8, NodePositions[7].first, NodePositions[7].second, {1, 23, 7, 9});
Nodes[9] = new Node(9, NodePositions[8].first, NodePositions[8].second, {2, 24, 8, 10});
Nodes[10] = new Node(10, NodePositions[9].first, NodePositions[9].second, {11, 25, 9, 27});
Nodes[11] = new Node(11, NodePositions[10].first, NodePositions[10].second, {12, 10, 2, 28});
Nodes[12] = new Node(12, NodePositions[11].first, NodePositions[11].second, {13, 11, 3, 29});
Nodes[13] = new Node(13, NodePositions[12].first, NodePositions[12].second, {32, 12, 14, 30});
Nodes[14] = new Node(14, NodePositions[13].first, NodePositions[13].second, {33, 3, 15, 13});
Nodes[15] = new Node(15, NodePositions[14].first, NodePositions[14].second, {34, 4, 16, 14});
Nodes[16] = new Node(16, NodePositions[15].first, NodePositions[15].second, {35, 5, 17, 15});
Nodes[17] = new Node(17, NodePositions[16].first, NodePositions[16].second, {36, 18, 38, 16});
Nodes[18] = new Node(18, NodePositions[17].first, NodePositions[17].second, {17, 19, 39, 5});
Nodes[19] = new Node(19, NodePositions[18].first, NodePositions[18].second, {18, 20, 40, 6});
Nodes[20] = new Node(20, NodePositions[19].first, NodePositions[19].second, {19, 21, 41, 7});
Nodes[21] = new Node(21, NodePositions[20].first, NodePositions[20].second, {20, 44, 42, 22});
Nodes[22] = new Node(22, NodePositions[21].first, NodePositions[21].second, {7, 45, 21, 23});
Nodes[23] = new Node(23, NodePositions[22].first, NodePositions[22].second, {8, 46, 22, 24});
Nodes[24] = new Node(24, NodePositions[23].first, NodePositions[23].second, {9, 47, 23, 25});
Nodes[25] = new Node(25, NodePositions[24].first, NodePositions[24].second, {10, 48, 24, 26});
Nodes[26] = new Node(26, NodePositions[25].first, NodePositions[25].second, {27, 49, 25});
Nodes[27] = new Node(27, NodePositions[26].first, NodePositions[26].second, {28, 26, 10});
Nodes[28] = new Node(28, NodePositions[27].first, NodePositions[27].second, {29, 27, 11});
Nodes[29] = new Node(29, NodePositions[28].first, NodePositions[28].second, {30, 28, 12});
Nodes[30] = new Node(30, NodePositions[29].first, NodePositions[29].second, {31, 29, 13});
Nodes[31] = new Node(31, NodePositions[30].first, NodePositions[30].second, {30, 32});
Nodes[32] = new Node(32, NodePositions[31].first, NodePositions[31].second, {13, 33, 31});
Nodes[33] = new Node(33, NodePositions[32].first, NodePositions[32].second, {14, 34, 32});
Nodes[34] = new Node(34, NodePositions[33].first, NodePositions[33].second, {15, 35, 33});
Nodes[35] = new Node(35, NodePositions[34].first, NodePositions[34].second, {16, 36, 34});
Nodes[36] = new Node(36, NodePositions[35].first, NodePositions[35].second, {17, 37, 35});
Nodes[37] = new Node(37, NodePositions[36].first, NodePositions[36].second, {38, 36});
Nodes[38] = new Node(38, NodePositions[37].first, NodePositions[37].second, {37, 39, 17});
Nodes[39] = new Node(39, NodePositions[38].first, NodePositions[38].second, {38, 40, 18});
Nodes[40] = new Node(40, NodePositions[39].first, NodePositions[39].second, {39, 41, 19});
Nodes[41] = new Node(41, NodePositions[40].first, NodePositions[40].second, {40, 42, 20});
Nodes[42] = new Node(42, NodePositions[41].first, NodePositions[41].second, {41, 43, 21});
Nodes[43] = new Node(43, NodePositions[42].first, NodePositions[42].second, {42, 44});
Nodes[44] = new Node(44, NodePositions[43].first, NodePositions[43].second, {21, 43, 45});
Nodes[45] = new Node(45, NodePositions[44].first, NodePositions[44].second, {22, 44, 46});
Nodes[46] = new Node(46, NodePositions[45].first, NodePositions[45].second, {23, 45, 47});
Nodes[47] = new Node(47, NodePositions[46].first, NodePositions[46].second, {24, 46, 48});
Nodes[48] = new Node(48, NodePositions[47].first, NodePositions[47].second, {25, 47, 49});
Nodes[49] = new Node(49, NodePositions[48].first, NodePositions[48].second, {26, 48});
```

## Topology Visualization

The following figure shows the **7x7 grid topology**, with Node 1 as the designated center node.

![7x7 Grid Topology](topology.png)
