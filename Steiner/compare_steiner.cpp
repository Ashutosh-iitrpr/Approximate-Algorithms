#include <iostream>
#include <vector>
#include <iomanip>
#include <tuple>
using namespace std;

#include "steiner_brute.cpp"      // Contains SteinerTreeBruteForce class
#include "steiner_tree_approx.cpp" // Contains SteinerTreeApprox class

int main() {
    cout << "  STEINER TREE ALGORITHM COMPARISON TOOL" << endl;
    int n, m;
    cout << "\nEnter number of vertices: ";
    cin >> n;
    
    cout << "Enter number of edges: ";
    cin >> m;
    
    cout << "\nEnter edges in format: u v weight" << endl;
    
    // Store edges to be used by both algorithms
    vector<tuple<int, int, int>> edges;
    
    for(int i = 0; i < m; i++) {
        int u, v, w;
        cout << "Edge " << (i+1) << ": ";
        cin >> u >> v >> w;
        edges.push_back({u, v, w});
        cout << "Edge added: " << u << " - " << v << " (weight: " << w << ")" << endl;
    }
    
    // Get terminal vertices
    int numTerminals;
    cout << "\nEnter number of terminal vertices: ";
    cin >> numTerminals;
    
    vector<int> terminals;
    cout << "Enter terminal vertices: ";
    for(int i = 0; i < numTerminals; i++) {
        int t;
        cin >> t;
        terminals.push_back(t);
    }
    
    
    SteinerTreeBruteForce bruteForce(n);
    
    for(auto& e : edges) {
        bruteForce.add_edge(get<0>(e), get<1>(e), get<2>(e));
    }
    
    bruteForce.set_terminals(terminals);
    bruteForce.solveSteinerTree();
    
    // Get results from brute force
    double bfTime = bruteForce.getExecutionTime();
    int bfCost = bruteForce.getMinCost();
    vector<pair<int,int>> bfTree = bruteForce.getBestSteinerTree();
    
    // ALGORITHM 2: 2-APPROXIMATION


    SteinerTreeApprox approx(n);
    
    for(auto& e : edges) {
        approx.add_edge(get<0>(e), get<1>(e), get<2>(e));
    }
    
    approx.set_terminals(terminals);
    approx.solveSteinerTree();
    
    // Get results from 2-approximation
    double approxTime = approx.getExecutionTime();
    int approxCost = approx.getMinCost();
    vector<pair<int,int>> approxTree = approx.getBestSteinerTree();

    cout << "  COMPARISON RESULTS" << endl;
    
    cout << fixed << setprecision(3);
    cout << "| Algorithm              | Runtime (ms)   | Tree Cost      |" << endl;
    
    cout << "| Brute Force (Optimal)  | " << setw(14) << bfTime
         << " | " << setw(14) << bfCost << " |" << endl;
    
    cout << "| 2-Approximation        | " << setw(14) << approxTime
         << " | " << setw(14) << approxCost << " |" << endl;
    
    return 0;
}
