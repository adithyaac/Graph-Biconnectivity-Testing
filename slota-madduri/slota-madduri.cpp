#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <chrono>
#include <fstream>
#include <string>

class Graph {
private:
    int vertices;
    std::vector<std::vector<int>> adj;

public:
    Graph(int v) : vertices(v) {
        adj.resize(v);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Undirected graph
    }

    // BFS to get parent (P) and level (L) information
    std::pair<std::vector<int>, std::vector<int>> BFS(int root) {
        std::vector<int> P(vertices, -1); // Parent array
        std::vector<int> L(vertices, -1); // Level array
        std::vector<bool> visited(vertices, false);
        
        std::queue<int> Q;
        Q.push(root);
        visited[root] = true;
        L[root] = 0;
        
        while (!Q.empty()) {
            int u = Q.front();
            Q.pop();
            
            for (int v : adj[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    P[v] = u;
                    L[v] = L[u] + 1;
                    Q.push(v);
                }
            }
        }
        
        return {P, L};
    }

    // Truncated BFS subroutine (Algorithm 6)
    int BFS_L(const std::vector<int>& L, int v, int u, std::vector<bool>& visited) {
        // Create a fresh visited array for this truncated BFS
        std::vector<bool> localVisited(vertices, false);
        std::queue<int> Q;
        std::set<int> S; // For tracking vertices to reset later
        
        Q.push(u);
        S.insert(u);
        S.insert(v);
        localVisited[u] = true;
        localVisited[v] = true; // Mark parent as visited to avoid going back
        
        while (!Q.empty()) {
            int x = Q.front();
            Q.pop();
            
            for (int w : adj[x]) {
                if (!localVisited[w]) {
                    if (L[w] < L[u]) {
                        // Found a back edge - can reach a vertex at a lower level
                        return L[w];
                    } else {
                        Q.push(w);
                        localVisited[w] = true;
                        S.insert(w);
                    }
                }
            }
        }
        
        // If we get here, no lower level was found
        return L[u];
    }

    // BFS-based algorithm to identify articulation points (Algorithm 5)
    std::vector<bool> BFS_ArtPts() {
        std::vector<bool> Art(vertices, false);
        std::vector<bool> dummyVisited(vertices, false);
        
        // Select a root vertex (using 0 as the root)
        int root = 0;
        
        // Get parent and level information from BFS
        auto [P, L] = BFS(root);
        
        // Check vertices other than root
        for (int u = 0; u < vertices; u++) {
            if (u != root && P[u] != root) { // Skip the root and direct children of root
                int v = P[u]; // Parent of u
                
                if (!Art[v]) {
                    int l = BFS_L(L, v, u, dummyVisited);
                    if (l >= L[v]) {
                        Art[v] = true;
                    }
                }
            }
        }
        
        // Check if root is an articulation point using a different approach
        // Root is an articulation point if and only if it has at least two children
        // that cannot reach each other without going through the root
        
        // First, get children of the root in the BFS tree
        std::vector<int> rootChildren;
        for (int u = 0; u < vertices; u++) {
            if (u != root && P[u] == root) {
                rootChildren.push_back(u);
            }
        }
        
        if (rootChildren.size() > 1) {
            // Create a subgraph without the root
            std::vector<bool> visited(vertices, false);
            visited[root] = true;  // Mark root as visited to exclude it
            
            // Start BFS from the first child
            std::queue<int> Q;
            Q.push(rootChildren[0]);
            visited[rootChildren[0]] = true;
            
            // Try to reach all other children
            while (!Q.empty()) {
                int u = Q.front();
                Q.pop();
                
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        visited[v] = true;
                        Q.push(v);
                    }
                }
            }
            
            // Check if all other children are reachable
            for (int child : rootChildren) {
                if (!visited[child]) {
                    // Found a child that cannot be reached, root is an articulation point
                    Art[root] = true;
                    break;
                }
            }
        }
        
        return Art;
    }

    // Check if the graph is biconnected
    bool isBiconnected() {
        // A graph with fewer than 3 vertices is trivially biconnected
        if (vertices <= 2) {
            return vertices == 2; // A single vertex is not biconnected by definition
        }
        
        // A graph is biconnected if it has no articulation points
        std::vector<bool> articulationPoints = BFS_ArtPts();
        
        for (bool isArt : articulationPoints) {
            if (isArt) {
                return false; // Found an articulation point, not biconnected
            }
        }
        
        // No articulation points found
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return 1;
    }
    
    int n, m;
    file >> n >> m; // Read number of vertices and edges
    
    Graph g(n);
    
    // Read edges
    for (int i = 0; i < m; i++) {
        int a, b;
        file >> a >> b;
        g.addEdge(a, b);
    }
    
    file.close();
    
    // Test if the graph is biconnected
    auto start = std::chrono::high_resolution_clock::now();
    bool result = g.isBiconnected();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    
    std::cout << "The graph is " << (result ? "biconnected" : "not biconnected") << std::endl;
    std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;
    
    return 0;
}
