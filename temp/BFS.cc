#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm> 

class Graph
{
private:
    int n;//节点数量
    std::vector<std::vector<int>> adj;//邻接表
public:
    Graph(int nodes) : n(nodes)
    {
        adj.resize(n);
    }

    void AddEdge(int u,int v)//添加无向边
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    void AddDirectedEdge(int u,int v)
    {
        adj[u].push_back(v);
    }

    void BFS(int s)
    {
        std::vector<bool> visited(n,false);
        std::queue<int>q;
        visited[s] = true;
        q.push(s);
        std::cout<<"BFS遍历顺序: ";
        while(!q.empty())
        {
            int u = q.front();
            q.pop();
            std::cout << u <<" ";
            //遍历所有邻接点
            for(int v : adj[u])
            {
                if(!visited[v]){
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        std::cout << std::endl;
    }
     // BFS 求最短路径（无权图）
    std::vector<int> BFSShortestPath(int s, int t)
    {
        std::vector<int> dist(n,-1);
        std::vector<int> prev(n,-1);
        std::queue<int> q;

        dist[s] = 0;
        q.push(s);
        while(!q.empty())
        {
            int u = q.front();
            q.pop();

            for(int v : adj[u])
            {
                if(dist[v] == -1)
                {
                    dist[v] = dist[u]+1;
                    prev[v] = u;
                    q.push(v);

                    if(v==t) break;
                }
            }
        }
        std::vector<int> path;
        if(dist[t] != -1)
        {
            for(int v = t; v != -1;v = prev[v])
            {
                path.push_back(v);
            }
            std::reverse(path.begin(),path.end());
        }
        return path;
    }
    // BFS 分层遍历
    void BFSWithLevels(int s) {
        std::vector<bool> visited(n, false);
        std::queue<int> q;
        
        visited[s] = true;
        q.push(s);
        
        int level = 0;
        while (!q.empty()) {
            int levelSize = q.size();
            std::cout << "第 " << level << " 层: ";
            
            for (int i = 0; i < levelSize; i++) {
                int u = q.front();
                q.pop();
                std::cout << u << " ";
                
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        visited[v] = true;
                        q.push(v);
                    }
                }
            }
            std::cout << std::endl;
            level++;
        }
    }
};