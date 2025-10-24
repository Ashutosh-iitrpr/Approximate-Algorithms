                                                        IMPLEMENTATION OF STEINER TREE
														
> We have implemented the finding of steiner tree through two algorithms 1.brute force 2.2-approximation algorithms which are in the files
  1.steiner_brute.cpp 
  2. steiner_tree_aprox.cpp 
Files Overview

1. STEINER_BRUTE.CPP


> This file contains the SteinerTreeBruteForce class that implements  brute‑force algorithm for finding the optimal Steiner tree.​
> This takes the number of vertices as input and has a built in functions to add the edges and terminal vertices.
  We have functions that converts the
  >Input graph to a metric closure using Floyd‑Warshall,
  >Creates subsets of non‑terminals with terminals
  >Builds an MST per subset in metric space
  > Replaces metric edges by original shortest paths, then removes cycles via Union‑Find to produce the steiner tree.​

2. STEINER_TREE_APROX.CPP

> This file contains the SteinerTreeApprox class that implements a 2‑approximation algorithm 
> This takes the number of vertices as input and has a built in functions to add the edges and terminal vertices.
  We have functions that converts the
  > converts input graph to metric closure with Floyd‑Warshall,
  > form complete graph on terminals,
  > compute MST on terminals in metric space, 
  > replace each MST edge with a shortest path in the original graph, remove cycles to yield the final tree.​

3. COMPARE_STENIER.CPP

>This file includes  both the above files as a header files and calls our required functions which are
  >> .getExecutionTime();
  >> .getMinCost();
  >> .getBestSteinerTree(); 

> This file  reads the graph and terminals, runs both algorithms, prints detailed intermediate steps, and outputs a side‑by‑side comparison of 
  runtime and tree cost.​

COMPILATION INSTRUCTIONS

>Prerequisites: a C++ compiler supporting at least C++11 (chrono, vectors, etc.) and standard libraries installed.​
>Compile each component as follows:

 1. Brute‑force algorithm:
 >>g++ -std=c++11 -O2 -o steiner_brute steiner_brute.cpp
 
 2. 2‑approximation algorithm:
 >>g++ -std=c++11 -O2 -o steiner_approx steiner_tree_aprox.cpp

 3. comparing the result:
 >>g++ -std=c++11 -O2 -o compare_steiner compare_stenier.cpp
   ./compare_steiner 
   
> you will see an Input Format like this :
 
 >>Number of vertices n, Enter the number of vertices
 >>Number of edges  m, Enter the number of edges 
 >>please note that your vertices start with 0 indexing 
 >>m lines: u v w for each undirected edge with weight w (with weight 1 for unweighted graphs),Enter like 0 1 2 means 
   0th vertex connecting 2 nd vertex with edge weight 2 (space sepereated)
 >>Number of terminal vertices k , Enter the number of vertices 
 >>The k terminal vertex indices (space‑separated).​

> the output will be like this:
 >> show the steiner tree of both algorithms ans their costs along with their execution times


 
  

