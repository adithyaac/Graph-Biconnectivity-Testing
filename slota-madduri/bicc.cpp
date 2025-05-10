#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <chrono>
#include <fstream>
#include <string>
#include <unordered_set>
#include <algorithm>

class Graph
{
private:
    int vertices;
    std::vector<std::vector<int>> adj;

public:
    Graph(int v) : vertices(v)
    {
        adj.resize(v);
    }

    void addEdge(int u, int v)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::tuple<std::vector<int>, std::vector<int>, std::vector<std::vector<int>>> BFS(int root)
    {
        std::vector<int> P(vertices, -1);
        std::vector<int> L(vertices, -1);
        std::vector<bool> visited(vertices, false);
        std::vector<std::vector<int>> LQ;

        std::queue<int> Q;
        Q.push(root);
        visited[root] = true;
        L[root] = 0;

        int maxLevel = 0;
        while (!Q.empty())
        {
            int u = Q.front();
            Q.pop();

            for (int v : adj[u])
            {
                if (!visited[v])
                {
                    visited[v] = true;
                    P[v] = u;
                    L[v] = L[u] + 1;
                    maxLevel = std::max(maxLevel, L[v]);
                    Q.push(v);
                }
            }
        }

        LQ.resize(maxLevel + 1);
        for (int v = 0; v < vertices; v++)
        {
            if (L[v] != -1)
            {
                LQ[L[v]].push_back(v);
            }
        }

        return {P, L, LQ};
    }

    std::tuple<int, int, std::vector<int>> BFS_LV(const std::vector<int> &L, int v, int u,
                                                  const std::vector<bool> &valid)
    {
        std::vector<bool> localVisited(vertices, false);
        std::queue<int> Q;
        std::vector<int> Vu;

        Q.push(u);
        Vu.push_back(u);
        localVisited[u] = true;
        localVisited[v] = true;

        int vidlow = u;

        while (!Q.empty())
        {
            int x = Q.front();
            Q.pop();

            for (int w : adj[x])
            {
                if (valid[w] && !localVisited[w])
                {
                    if (L[w] < L[u])
                    {
                        return {L[w], 0, std::vector<int>()};
                    }
                    else
                    {
                        Q.push(w);
                        Vu.push_back(w);
                        localVisited[w] = true;
                        if (w < vidlow)
                        {
                            vidlow = w;
                        }
                    }
                }
            }
        }

        return {L[u], vidlow, Vu};
    }

    int BFS_BICC()
    {
        std::vector<bool> Art(vertices, false);
        std::vector<bool> valid(vertices, true); 
        std::vector<int> Low(vertices);
        std::vector<int> Par(vertices);
        std::map<std::pair<int, int>, int> BiCC;    
        std::vector<bool> processed(vertices, false); 
        std::unordered_set<int> componentIds;
        int connectedComponents = 0;

        for (int v = 0; v < vertices; v++)
        {
            Low[v] = v;
            Par[v] = v;
        }

        for (int startVertex = 0; startVertex < vertices; startVertex++)
        {
            if (processed[startVertex])
                continue; 

            connectedComponents++;
            int root = startVertex;

            auto [P, L, LQ] = BFS(root);

            for (int v = 0; v < vertices; v++)
            {
                if (L[v] != -1)
                {
                    processed[v] = true;
                }
            }

            for (int i = LQ.size() - 1; i >= 0; i--)
            {
                auto &Qi = LQ[i];

                for (auto it = Qi.begin(); it != Qi.end();)
                {
                    int u = *it;

                    if (Par[u] == u && valid[u])
                    {                      
                        it = Qi.erase(it); 

                        if (u != root)
                        {                 
                            int v = P[u]; 

                            auto [l, vidlow, Vu] = BFS_LV(L, v, u, valid);

                            if (l >= L[v])
                            {
                                Art[v] = true;

                                for (int w : Vu)
                                {
                                    Low[w] = vidlow;
                                    Par[w] = v;
                                    valid[w] = false; 
                                }
                            }
                        }
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            for (int u = 0; u < vertices; u++)
            {
                if (L[u] == -1)
                    continue; 

                for (int v : adj[u])
                {
                    if (L[v] == -1)
                        continue; 

                    if (u < v)
                    {
                        if (Low[v] == Low[u] || Par[u] == v)
                        {
                            BiCC[{u, v}] = Low[u];
                            componentIds.insert(Low[u]);
                        }
                        else
                        {
                            BiCC[{u, v}] = Low[v];
                            componentIds.insert(Low[v]);
                        }
                    }
                }
            }
        }

        return componentIds.size();
    }
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return 1;
    }

    int n, m;
    file >> n >> m;

    Graph g(n);

    for (int i = 0; i < m; i++)
    {
        int a, b;
        file >> a >> b;
        g.addEdge(a, b);
    }

    file.close();

    if (n == 0)
    {
        std::cout << "Number of biconnected components: 0" << std::endl;
        std::cout << "Execution time: 0 milliseconds" << std::endl;
        return 0;
    }

    if (n == 1)
    {
        std::cout << "Number of biconnected components: 0" << std::endl;
        std::cout << "Execution time: 0 milliseconds" << std::endl;
        return 0;
    }

    if (n == 2 && m > 0)
    {
        std::cout << "Number of biconnected components: 1" << std::endl;
        std::cout << "Execution time: 0 milliseconds" << std::endl;
        return 0;
    }

    auto start = std::chrono::high_resolution_clock::now();
    int numComponents = g.BFS_BICC();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Number of biconnected components: " << numComponents << std::endl;
    std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}
