#include <iostream>
#include <vector>

class GraphDFS
{
private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<bool> visited;

    void DFSUtil(int u)
    {
        visited[u] = true;
        for(int v:adj[u])
        {
            if(!visited[v])
            {
                DFSUtil(v);
            }
        }
    }
public:
    GraphDFS(int nodes) : n(nodes) {
        adj.resize(n);
    }
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);  // 无向图
    }
    void DFS(int s) {
        visited.assign(n, false);
        std::cout << "DFS遍历顺序: ";
        DFSUtil(s);
        std::cout << std::endl;
    }
    // 遍历整个图（处理非连通图）
    void DFSFull() {
        visited.assign(n, false);
        std::cout << "完整DFS遍历: ";
        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                DFSUtil(i);
            }
        }
        std::cout << std::endl;
    }
    // 判断是否存在路径
    bool hasPath(int s, int t) {
        visited.assign(n, false);
        return hasPathUtil(s, t);
    }
private:
    bool hasPathUtil(int u,int t)
    {
        if(u==t) return true;

        visited[u] = true;

        for(int v: adj[u])
        {
            if(!visited[v])
            {
                if(hasPathUtil(v,t))
                {
                    return true;
                }
            }
        }
        return false;
    }
};