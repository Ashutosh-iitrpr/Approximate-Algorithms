#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <set>
#include <map>
#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

class SteinerTreeBruteForce {
private:
    int n;  // number of vertices
    vector<vector<int>> graph;   // adjacency matrix of original graph
    vector<vector<int>> metric;  // metric graph (complete graph with shortest paths)
    vector<int> terminals;       // terminal vertices
    vector<bool> isTerminal;     //  set of  array for terminal vertices
    double executionTime;        // runtime in milliseconds
    int minCost;                 // minimum cost found
    vector<pair<int,int>> bestSteinerTree;  // best Steiner tree edges
    vector<int> bestSteinerVertices;        // vertices in best Steiner tree
    vector<pair<int,int>> finalTreeEdges;   // final tree after cycle removal

public:
    SteinerTreeBruteForce(int vertices) {
        n = vertices;
        graph.resize(n, vector<int>(n, INT_MAX));
        metric.resize(n, vector<int>(n, INT_MAX));
        isTerminal.resize(n, false);
        executionTime = 0.0;
        minCost = INT_MAX;
        
        for(int i = 0; i < n; i++) {
            graph[i][i] = 0;
            metric[i][i] = 0;
        }
    }
    
    void add_edge(int u, int v, int weight) { // adding our actual edges  
        if(u >= 0 && u < n && v >= 0 && v < n) {
            graph[u][v] = min(graph[u][v], weight);
            graph[v][u] = min(graph[v][u], weight);
        }
    }
    
    void set_terminals(vector<int>& term) {
        terminals = term;
        for(int t : terminals) {
            if(t >= 0 && t < n) {
                isTerminal[t] = true;
            }
        }
        cout << "Terminal vertices set: ";
        for(int t : terminals) cout << t << " ";
        cout << endl;
    }
    
    // Convert graph to metric graph using Floyd-Warshall 
    void convert_metric() {
        cout << "\n Converting to Metric Graph using Floyd-Warshall" << endl;
        
        // Copying  original graph
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                metric[i][j] = graph[i][j];
            }
        }
        
        // Floyd-Warshall algorithm
        for(int k = 0; k < n; k++) {
            for(int i = 0; i < n; i++) {
                for(int j = 0; j < n; j++) {
                    if(metric[i][k] != INT_MAX && metric[k][j] != INT_MAX) {
                        if(metric[i][k] + metric[k][j] < metric[i][j]) {
                            metric[i][j] = metric[i][k] + metric[k][j];
                        }
                    }
                }
            }
        }
        
        cout << "Metric Graph computed successfully." << endl;
        cout << "\nMetric Graph:" << endl;
        cout << "     ";
        for(int j = 0; j < n; j++) cout << j << "\t";
        cout << endl;
        
        for(int i = 0; i < n; i++) {
            cout << i << ":   ";
            for(int j = 0; j < n; j++) {
                if(metric[i][j] == INT_MAX)
                    cout << "INF\t";
                else
                    cout << metric[i][j] << "\t";
            }
            cout << endl;
        }
    }
    
    // Extracting the subgraph from metric graph for given vertex set
    vector<vector<int>> extract_subgraph(vector<int>& vertices) {
        int m = vertices.size();
        vector<vector<int>> subgraph(m, vector<int>(m, INT_MAX));// initiaiting our subgraph 
        
        for(int i = 0; i < m; i++) {// adding those crresponding edges to our subgrpah from metric graph
            for(int j = 0; j < m; j++) {
                int u = vertices[i];
                int v = vertices[j];
                subgraph[i][j] = metric[u][v];
            }
        }
        
        return subgraph;
    }
    
    // Computing  MST on subgraph using Prim's algorithm 
    // this function returns MST cost and edges in terms of original vertex indices
    pair<int, vector<pair<int,int>>> compute_mst_on_subgraph(vector<int>& vertices) {
        int m = vertices.size();
        if(m == 0) return {0, {}};
        
        vector<vector<int>> subgraph = extract_subgraph(vertices);
        
        vector<int> parent(m, -1);
        vector<int> key(m, INT_MAX);
        vector<bool> inMST(m, false);
        
        key[0] = 0;
        int mstCost = 0;
        vector<pair<int,int>> mstEdges;
        
        for(int count = 0; count < m; count++) {
            // Find minimum key vertex not in MST
            int u = -1;
            int minKey = INT_MAX;
            
            for(int v = 0; v < m; v++) {
                if(!inMST[v] && key[v] < minKey) {
                    minKey = key[v];
                    u = v;
                }
            }
            
            if(u == -1) break;
            
            inMST[u] = true;
            
            // Add edge to MST
            if(parent[u] != -1) {
                int originalU = vertices[parent[u]];
                int originalV = vertices[u];
                mstEdges.push_back({originalU, originalV});
                mstCost += subgraph[parent[u]][u];
            }
            
            // Update keys
            for(int v = 0; v < m; v++) {
                if(!inMST[v] && subgraph[u][v] != INT_MAX && subgraph[u][v] < key[v]) {
                    key[v] = subgraph[u][v];
                    parent[v] = u;
                }
            }
        }
        
        return {mstCost, mstEdges};
    }
    
    vector<int> get_non_terminals() {
        vector<int> nonTerminals;
        for(int i = 0; i < n; i++) {
            if(!isTerminal[i]) {
                nonTerminals.push_back(i);
            }
        }
        return nonTerminals;
    }
    
    //Generating  all possible subsets of  non-terminal vertices to make union with the terminal vertices
    // Generate all subsets of non-terminals using bit masking
    void get_all_subsets() {
        cout << "\n making all Subsets of Non-Terminal Vertices" << endl;
        
        vector<int> nonTerminals = get_non_terminals();
        int numof_nonTerminals = nonTerminals.size();    
        int totalSubsets = (1 << numof_nonTerminals);// 2 power of non terminals
        
        for(int k = 0; k < totalSubsets; k++) {
            // Creating union of terminals and current subset of non-terminals
            vector<int> currentVertices = terminals;
            
            for(int i = 0; i < numof_nonTerminals; i++) {
                if(k & (1 << i)) {
                    currentVertices.push_back(nonTerminals[i]);
                }
            }
            
            // Compute MST on this subset
            auto result = compute_mst_on_subgraph(currentVertices);
            int currentCost = result.first;
            vector<pair<int,int>> currentMSTEdges = result.second;
            
            // Update best solution if this is better
            if(currentCost < minCost) {
                minCost = currentCost;
                bestSteinerTree = currentMSTEdges;
                bestSteinerVertices = currentVertices;
            }
        }
        
        cout << "Enumeration complete. Best cost found: " << minCost << endl;
    }
    
    // Reconstructing actual paths in original graph for each metric edge
    vector<int> find_path_in_original(int u, int v) {
        // Simple Dijkstra to find actual shortest path
        vector<int> parent(n, -1);
        vector<int> dist(n, INT_MAX);
        vector<bool> visited(n, false);
        
        dist[u] = 0;
        
        for(int count = 0; count < n; count++) {
            int minDist = INT_MAX;
            int current = -1;
            
            for(int i = 0; i < n; i++) {
                if(!visited[i] && dist[i] < minDist) {
                    minDist = dist[i];
                    current = i;
                }
            }
            
            if(current == -1 || current == v) break;
            visited[current] = true;
            
            for(int next = 0; next < n; next++) {
                if(graph[current][next] != INT_MAX && graph[current][next] > 0) {
                    if(dist[current] + graph[current][next] < dist[next]) {
                        dist[next] = dist[current] + graph[current][next];
                        parent[next] = current;
                    }
                }
            }
        }
        
        // Reconstruct path
        vector<int> path;
        int curr = v;
        while(curr != -1) {
            path.push_back(curr);
            curr = parent[curr];
        }
        reverse(path.begin(), path.end());
        
        return path;
    }
    
    // Replace metric edges with actual paths and collect all edges
    vector<pair<int,int>> replace_with_actual_paths() {
        cout << "\n=== Replacing Metric Edges with Actual Paths ===" << endl;
        
        set<pair<int,int>> actualEdgesSet;  // Use set to avoid duplicate edges
        set<int> allVertices;
        
        for(auto& edge : bestSteinerTree) {
            int u = edge.first;
            int v = edge.second;
            
            // Find actual path
            vector<int> path = find_path_in_original(u, v);
            
            if(path.size() >= 2) {
                cout << "Path from " << u << " to " << v << ": ";
                for(int i = 0; i < (int)path.size() - 1; i++) {
                    int a = path[i];
                    int b = path[i+1];
                    
                    // Add vertices
                    allVertices.insert(a);
                    allVertices.insert(b);
                    
                    // Add edge in canonical form (smaller vertex first)
                    if(a > b) swap(a, b);
                    actualEdgesSet.insert({a, b});
                    
                    cout << path[i] << " -> ";
                }
                cout << path.back() << endl;
            }
        }
        
        
        
        // Convert set to vector
        vector<pair<int,int>> actualEdges(actualEdgesSet.begin(), actualEdgesSet.end());
        
        // Remove cycles to get final tree
        finalTreeEdges = remove_cycles(actualEdges, allVertices);
        
        return finalTreeEdges;
    }
    
    // Remove cycles using Union-Find to get a valid tree
    vector<pair<int,int>> remove_cycles(vector<pair<int,int>>& edges, set<int>& vertices) {
        cout << "\n=== Removing Cycles to Form Valid Steiner Tree ===" << endl;
        
        // Union-Find data structure
        map<int, int> parent;
        map<int, int> rank;
        
        // Initialize Union-Find
        for(int v : vertices) {
            parent[v] = v;
            rank[v] = 0;
        }
        
        // Find with path compression
        function<int(int)> find = [&](int x) -> int {
            if(parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        };
        
        // Union by rank
        auto unite = [&](int x, int y) -> bool {
            int px = find(x);
            int py = find(y);
            
            if(px == py) return false;  // Would create cycle
            
            if(rank[px] < rank[py]) {
                parent[px] = py;
            } else if(rank[px] > rank[py]) {
                parent[py] = px;
            } else {
                parent[py] = px;
                rank[px]++;
            }
            return true;
        };
        
        vector<pair<int,int>> treeEdges;
        int cyclesRemoved = 0;
        
        // Add edges one by one, skipping those that create cycles
        for(auto& edge : edges) {
            if(unite(edge.first, edge.second)) {
                treeEdges.push_back(edge);
            } else {
                cout << "  Removing edge " << edge.first << " - " << edge.second 
                     << " (creates cycle)" << endl;
                cyclesRemoved++;
            }
        }
        
        if(cyclesRemoved == 0) {
            cout << "No cycles detected. Tree is valid." << endl;
        } else {
            cout << "Removed " << cyclesRemoved << " edge(s) that created cycles." << endl;
        }
        
        cout << "Final tree has " << treeEdges.size() << " edges." << endl;
        
        return treeEdges;
    }
    
    void displaySteinerTree() {
        cout << "\n=== FINAL STEINER TREE SOLUTION ===" << endl;
        
        cout << "\nTerminal vertices: ";
        for(int t : terminals) cout << t << " ";
        cout << endl;
        
        cout << "\nSteiner vertices used: ";
        bool hasSteiner = false;
        for(int v : bestSteinerVertices) {
            if(!isTerminal[v]) {
                cout << v << " ";
                hasSteiner = true;
            }
        }
        if(!hasSteiner) cout << "(none)";
        cout << endl;
        
        cout << "\nMetric MST edges (before path replacement):" << endl;
        for(auto& edge : bestSteinerTree) {
            cout << "  " << edge.first << " - " << edge.second 
                 << " (metric weight: " << metric[edge.first][edge.second] << ")" << endl;
        }
        
        // Replace with actual paths and remove cycles
        vector<pair<int,int>> actualEdges = replace_with_actual_paths();
        
        cout << "\n=== Final Steiner Tree Edges (after cycle removal) ===" << endl;
        int totalCost = 0;
        for(auto& edge : actualEdges) {
            int w = graph[edge.first][edge.second];
            cout << "  " << edge.first << " - " << edge.second 
                 << " (weight: " << w << ")" << endl;
            totalCost += w;
        }
        
        cout << "\n=== COST ANALYSIS ===" << endl;
        cout << "Metric MST cost: " << minCost << endl;
        cout << "Final Steiner Tree cost: " << totalCost << endl;
        
        // Verify it's a valid tree
        int numEdges = actualEdges.size();
        set<int> vertexSet;
        for(auto& edge : actualEdges) {
            vertexSet.insert(edge.first);
            vertexSet.insert(edge.second);
        }
        int numVertices = vertexSet.size();
        
        cout << "\nTree Validation:" << endl;
        cout << "  Number of vertices: " << numVertices << endl;
        cout << "  Number of edges: " << numEdges << endl;
        if(numEdges == numVertices - 1) {
            cout << "  Status: VALID TREE (edges = vertices - 1)" << endl;
        } else {
            cout << "  Status: WARNING - Not a valid tree structure!" << endl;
        }
    }
    
    void displayRuntime() {
        cout << "\n=== RUNTIME ANALYSIS ===" << endl;
        cout << fixed << setprecision(3);
        cout << "Total Execution Time: " << executionTime << " ms" << endl;
    }
    
    void solveSteinerTree() {
       
        cout << "  STEINER TREE SOLVER - BRUTE FORCE" << endl;
      

        auto start = high_resolution_clock::now();
        
        // Convert to metric graph
        convert_metric();
        // Making all subsets and find best MST
        get_all_subsets();
        
        auto end = high_resolution_clock::now();
        executionTime = duration_cast<microseconds>(end - start).count() / 1000.0;
        
        // Display results
        displaySteinerTree();
        displayRuntime();
    }
    
    int getMinCost() { return minCost; }
    double getExecutionTime() { return executionTime; }
    vector<pair<int,int>> getBestSteinerTree() { return finalTreeEdges; }
};



