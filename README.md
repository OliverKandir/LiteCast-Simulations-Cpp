This repository contains Various Simulations written in c++ (Autor: Oliver Kandir) to study, verify and improve the working of the published protocol 

LiteCast: Flexible Scalable and Uniform Local Data-Sharing in Real-Time (DOI: 10.1109/ICCCN58024.2023.10230145)

Folder #0 Initial Experimental Codes.. contains Simulation focusing on reproducing the results of the Six Node network Trace(Given in Fig.4 of the paper). 
One with Load Balancing done explicitly to match the Paper Results and One with Load Balancing feature completely removed. Both of the Results are also given in
txt files.

Folder #1 Contains configurable General code of the Simulation that can work on any topology (for details on how to configure Read README.md inside the Folder). 
For a list of tested topologies check the Example Topologies folder.

Folder #2 Contains the General code of the Simulation in a single c++ file, It is still configurable but requires a lot of scrolling around.

Folder #3 Contains the General code of the Simulation with a little modification on How a node discards a packet (Attempt to incorporate TTL mechanism has been attempted). The mechanism worked perfectly in ensuring a node captures only LC-n data (i.e capture from all Nodes in the Locality), but introduction of this mechanism led to conflict in working of Constructive Interference for more details see the Presentation Explaining this conflict.

Folder #5 Contains the General Code simulation run on DCube Topology with varying slot sizes so to reproduce the results of Fig.11 of the the paper.
