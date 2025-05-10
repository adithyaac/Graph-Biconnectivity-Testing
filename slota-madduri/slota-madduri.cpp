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
        adj[v].push_back(u);
    }

    std::pair<std::vector<int>, std::vector<int>> BFS(int root) {
        std::vector<int> P(vertices, -1); 
        std::vector<int> L(vertices, -1); 
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

    int BFS_L(const std::vector<int>& L, int v, int u, std::vector<bool>& visited) {
        std::vector<bool> localVisited(vertices, false);
        std::queue<int> Q;
        std::set<int> S; 
        
        Q.push(u);
        S.insert(u);
        S.insert(v);
        localVisited[u] = true;
        localVisited[v] = true; 
        
        while (!Q.empty()) {
            int x = Q.front();
            Q.pop();
            
            for (int w : adj[x]) {
                if (!localVisited[w]) {
                    if (L[w] < L[u]) {
                        return L[w];
                    } else {
                        Q.push(w);
                        localVisited[w] = true;
                        S.insert(w);
                    }
                }
            }
        }
        
        return L[u];
    }

    std::vector<bool> BFS_ArtPts() {
        std::vector<bool> Art(vertices, false);
        std::vector<bool> dummyVisited(vertices, false);
        
        int root = 0;
        
        auto [P, L] = BFS(root);
        
        for (int u = 0; u < vertices; u++) {
            if (u != root && P[u] != root) { 
                int v = P[u]; 
                
                if (!Art[v]) {
                    int l = BFS_L(L, v, u, dummyVisited);
                    if (l > L[v]) {
                        Art[v] = true;
                    }
                }
            }
        }
        
        std::vector<int> rootChildren;
        for (int u = 0; u < vertices; u++) {
            if (u != root && P[u] == root) {
                rootChildren.push_back(u);
            }
        }
        
        if (rootChildren.size() > 1) {
            std::vector<bool> visited(vertices, false);
            visited[root] = true;  
            
            std::queue<int> Q;
            Q.push(rootChildren[0]);
            visited[rootChildren[0]] = true;
            
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
            
            for (int child : rootChildren) {
                if (!visited[child]) {
                    Art[root] = true;
                    break;
                }
            }
        }
        
        return Art;
    }

    bool isBiconnected() {
        if (vertices <= 2) {
            return vertices == 2;
        }
        
        std::vector<bool> articulationPoints = BFS_ArtPts();
        
        for (bool isArt : articulationPoints) {
            if (isArt) {
                return false; 
            }
        }
        
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
    file >> n >> m;
    
    Graph g(n);
    
    for (int i = 0; i < m; i++) {
        int a, b;
        file >> a >> b;
        g.addEdge(a, b);
    }
    
    file.close();
    
    auto start = std::chrono::high_resolution_clock::now();
    bool result = g.isBiconnected();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    
    std::cout << "The graph is " << (result ? "biconnected" : "not biconnected") << std::endl;
    std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;
    
    return 0;
}
