# LiteCast Simulations

This repository contains various simulations written in C++ by **Oliver Kandir** to study, verify, and improve the implementation of the published protocol:

**LiteCast: Flexible, Scalable, and Uniform Local Data-Sharing in Real-Time**  
DOI: 10.1109/ICCCN58024.2023.10230145

## Repository Structure

### #0 Initial Experimental Codes

This folder contains simulations focused on reproducing the results of the **six-node network trace** given in **Fig. 4** of the paper.

Two implementations are provided:

- One with **Load Balancing explicitly implemented** to match the results presented in the paper.
- One with the **Load Balancing feature completely removed**.

The corresponding simulation results are also provided as `.txt` files.

### #1 General Configurable Simulation

This folder contains the **general configurable simulation code**, which can work with different network topologies.

For details on how to configure the simulation, refer to the [README.md](./%231%20Configurable%20General%20Code%20OHC%20Based/README.md) inside the folder.

For a list of tested topologies, refer to the [Example Topologies](./Example%20Topologies/) folder.

### #2 Single-File General Simulation

This folder contains the **general simulation code in a single C++ file**.

The simulation is still configurable; however, configuration requires navigating through and modifying the relevant sections of the code.

### #3 Simulation with Packet Discard Modification

This folder contains the **general simulation code with a modification to how a node discards a packet**.

An attempt was made to incorporate a **TTL (Time-to-Live) mechanism**. The mechanism worked as intended in ensuring that a node captures only **LC-n data**, i.e., data captured from all nodes within the locality.

However, introducing this mechanism resulted in a conflict with the operation of **Constructive Interference**.

For more details, refer to the **Presentation Explaining This Conflict**.

### #4 DCube Topology Simulation

This folder contains the **general simulation code executed on the DCube topology with varying slot sizes** to reproduce the results presented in **Fig. 11** of the paper.
