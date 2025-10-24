#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <set>
#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

class SteinerTreeApprox {
private:
    int n;  
    vector<vector<int>> graph;  
    vector<vector<int>> metric;  
    vector<vector<int>> parent;  
    vector<int> terminals;      
    vector<bool> isTerminal;     
    double executionTime;        
    int minCost;                 
    vector<pair<int,int>> bestSteinerTree; 
    vector<int> bestSteinerVertices;   

public:
    SteinerTreeApprox(int vertices) {
        n = vertices;
        graph.resize(n, vector<int>(n, INT_MAX));
        metric.resize(n, vector<int>(n, INT_MAX));
        parent.resize(n, vector<int>(n, -1));
        isTerminal.resize(n, false);
        executionTime = 0.0;
        minCost = INT_MAX;
        
        for(int i = 0; i < n; i++) {
            graph[i][i] = 0;
            metric[i][i] = 0;
            parent[i][i] = i;
        }
    }
    
    void add_edge(int u, int v, int weight) {
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
        cout << "Terminal vertices (R) set: ";
        for(int t : terminals) cout << t << " ";
        cout << endl;
    }
    
    // Step 1: Convert graph to metric closure using Floyd-Warshall
    // Also maintain parent matrix for path reconstruction
    void convert_metric() {
        cout << "\n=== Step 1: Computing Metric Closure ===" << endl;
   
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                metric[i][j] = graph[i][j];
                if(i != j && graph[i][j] != INT_MAX) {
                    parent[i][j] = i;  // direct edge
                }
            }
        }
        
        // Floyd-Warshall algorithm with path reconstruction
        for(int k = 0; k < n; k++) {
            for(int i = 0; i < n; i++) {
                for(int j = 0; j < n; j++) {
                    if(metric[i][k] != INT_MAX && metric[k][j] != INT_MAX) {
                        if(metric[i][k] + metric[k][j] < metric[i][j]) {
                            metric[i][j] = metric[i][k] + metric[k][j];
                            parent[i][j] = parent[k][j]; 
                        }
                    }
                }
            }
        }
        
        cout << "Metric closure computed successfully." << endl;
        cout << "\nMetric Graph (all-pairs shortest distances):" << endl;
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
    
    // Step 2: Extract complete graph KR on terminals R
    vector<vector<int>> extract_terminal_subgraph() {
        cout << "\nStep 2: Forming Complete Graph K_R on Terminals" << endl;
        
        int m = terminals.size();
        vector<vector<int>> KR(m, vector<int>(m, INT_MAX));
        
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < m; j++) {
                int u = terminals[i];
                int v = terminals[j];
                KR[i][j] = metric[u][v];
            }
        }
        
        cout << "Complete graph K_R formed with " << m << " terminals." << endl;
        cout << "\nK_R adjacency matrix:" << endl;
        cout << "     ";
        for(int j = 0; j < m; j++) cout << terminals[j] << "\t";
        cout << endl;
        
        for(int i = 0; i < m; i++) {
            cout << terminals[i] << ":   ";
            for(int j = 0; j < m; j++) {
                if(KR[i][j] == INT_MAX)
                    cout << "INF\t";
                else
                    cout << KR[i][j] << "\t";
            }
            cout << endl;
        }
        
        return KR;
    }
    
    // Step 3: Compute MST on KR using Prim's algorithm
    vector<pair<int,int>> compute_mst_on_terminals() {
        cout << "\n=== Step 3: Computing MST T_R on K_R ===" << endl;
        
        int m = terminals.size();
        vector<vector<int>> KR = extract_terminal_subgraph();
        
        vector<int> parentMST(m, -1);
        vector<int> key(m, INT_MAX);
        vector<bool> inMST(m, false);
        
        key[0] = 0;
        int mstCost = 0;
        vector<pair<int,int>> mstEdges;
        
        for(int count = 0; count < m; count++) {
 
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

            if(parentMST[u] != -1) {
                int originalU = terminals[parentMST[u]];
                int originalV = terminals[u];
                mstEdges.push_back({originalU, originalV});
                mstCost += KR[parentMST[u]][u];
            }

            for(int v = 0; v < m; v++) {
                if(!inMST[v] && KR[u][v] != INT_MAX && KR[u][v] < key[v]) {
                    key[v] = KR[u][v];
                    parentMST[v] = u;
                }
            }
        }
        
        cout << "MST T_R computed with cost: " << mstCost << endl;
        cout << "\nMST T_R edges (in metric space):" << endl;
        for(auto& edge : mstEdges) {
            cout << "  " << edge.first << " - " << edge.second 
                 << " (metric distance: " << metric[edge.first][edge.second] << ")" << endl;
        }
        
        minCost = mstCost;
        return mstEdges;
    }
    
    // Reconstruct shortest path from u to v using parent matrix
    vector<int> reconstruct_path(int u, int v) {
        if(metric[u][v] == INT_MAX) return {};
        
        vector<int> path;
        function<void(int, int)> build_path = [&](int s, int t) {
            if(s == t) {
                path.push_back(s);
                return;
            }
            build_path(s, parent[s][t]);
            path.push_back(t);
        };
        
        build_path(u, v);
        return path;
    }
    
    // Step 4: Replace each MST edge with shortest path and remove cycles
    vector<pair<int,int>> replace_with_shortest_paths(vector<pair<int,int>>& mstEdges) {
        cout << "\n=== Step 4: Replacing MST Edges with Shortest Paths ===" << endl;
        
        set<pair<int,int>> allEdges;  
        set<int> allVertices;
        
        for(auto& edge : mstEdges) {
            int u = edge.first;
            int v = edge.second;
            

            vector<int> path = reconstruct_path(u, v);
            
            cout << "Shortest path from " << u << " to " << v << ": ";
            for(int i = 0; i < (int)path.size(); i++) {
                cout << path[i];
                if(i < (int)path.size() - 1) cout << " -> ";
                allVertices.insert(path[i]);
            }
            cout << endl;
            
   
            for(int i = 0; i < (int)path.size() - 1; i++) {
                int a = path[i];
                int b = path[i+1];
          
                if(a > b) swap(a, b);
                allEdges.insert({a, b});
            }
        }

        vector<pair<int,int>> pathEdges(allEdges.begin(), allEdges.end());
        
        cout << "\nUnion of all shortest paths contains " << allEdges.size() 
             << " edges and " << allVertices.size() << " vertices." << endl;
        
        vector<pair<int,int>> finalTree = remove_cycles(pathEdges, allVertices);
        
        bestSteinerTree = finalTree;
        bestSteinerVertices = vector<int>(allVertices.begin(), allVertices.end());
        
        return finalTree;
    }
    
    vector<pair<int,int>> remove_cycles(vector<pair<int,int>>& edges, set<int>& vertices) {
        cout << "\n=== Removing Cycles to Form Tree T ===" << endl;
        
        // Use Union-Find to detect and remove cycles
        map<int, int> parent;
        map<int, int> rank;
        
        for(int v : vertices) {
            parent[v] = v;
            rank[v] = 0;
        }
        
        function<int(int)> find = [&](int x) {
            if(parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        };
        
        auto unite = [&](int x, int y) -> bool {
            int px = find(x);
            int py = find(y);
            
            if(px == py) return false; 
            
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
        
        for(auto& edge : edges) {
            if(unite(edge.first, edge.second)) {
                treeEdges.push_back(edge);
            } else {
                cout << "  Removing edge " << edge.first << " - " << edge.second 
                     << " (creates cycle)" << endl;
            }
        }
        
        cout << "Tree T formed with " << treeEdges.size() << " edges." << endl;
        
        return treeEdges;
    }
    
    void displaySteinerTree() {
        cout << "\n=== Step 5: OUTPUT - Final Steiner Tree T ===" << endl;
        
        cout << "\nTerminal vertices (R): ";
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
        
        cout << "\nFinal Steiner Tree edges:" << endl;
        int totalCost = 0;
        for(auto& edge : bestSteinerTree) {
            int w = graph[edge.first][edge.second];
            cout << "  " << edge.first << " - " << edge.second 
                 << " (weight: " << w << ")" << endl;
            totalCost += w;
        }
        
        cout << "\n=== STEINER TREE COST ===" << endl;
        cout << "MST on terminals cost (metric): " << minCost << endl;
        cout << "Final tree cost (actual edges): " << totalCost << endl;
    }
    
    void displayRuntime() {
        cout << "\n=== RUNTIME ANALYSIS ===" << endl;
        cout << fixed << setprecision(3);
        cout << "Total Execution Time: " << executionTime << " ms" << endl;
    }
    
    void solveSteinerTree() {
        
        cout << "  STEINER TREE - 2-APPROXIMATION ALGORITHM" << endl;
        
        
        auto start = high_resolution_clock::now();
        
        convert_metric();
        
        vector<pair<int,int>> mstEdges = compute_mst_on_terminals();
        
        replace_with_shortest_paths(mstEdges);
        
        auto end = high_resolution_clock::now();
        executionTime = duration_cast<microseconds>(end - start).count() / 1000.0;
        
        // Step 5: Display results
        displaySteinerTree();
        displayRuntime();
    }
    
    int getMinCost() { return minCost; }
    double getExecutionTime() { return executionTime; }
    vector<pair<int,int>> getBestSteinerTree() { return bestSteinerTree; }
};


