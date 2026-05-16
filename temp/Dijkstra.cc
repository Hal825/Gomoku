#include <iostream>
#include <vector>
#include <climits>
#include <queue>

typedef std::pair<int,int> pii;//<距离，节点编号>

class Dijkstra
{
private:
    int n;
    std::vector<std::vector<pii>> graph; //邻接表:graph[u] = {v,weight}
public:
    Dijkstra(int nodes):n(nodes)
    {
        graph.resize(n);
    }

    void AddEdge(int u,int v,int w)//添加有向边u->v,权重为w
    {
        graph[u].push_back({v,w});
    }

    void AddUndirectedEdge(int u,int v,int w)
    {
        graph[u].push_back({v,w});
        graph[v].push_back({u,w});
    }
    //计算从源点s到所有点的最短距离
    std::vector<int> ShortestPath(int s)
    {
        std::vector<int>dist(n,INT_MAX);
        dist[s] = 0;
        //优先队列，最小堆
        std::priority_queue<pii,std::vector<pii>,std::greater<pii>> pq;
        pq.push({0,s});//当前距离，当前节点
        
        while(!pq.empty())
        {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if(d>dist[u]) continue;

            for(auto& edge:graph[u])
            {
                int v = edge.first;
                int w = edge.second;
                //松弛操作
                if(dist[u] + w < dist[v])
                {
                    dist[v] = dist[u]+w;
                    pq.push({dist[v],v});
                }
            }
        }
        return dist;
    }
    int OneShortestPath(int s,int t)
    {
        std::vector<int> dist = ShortestPath(s);
        return dist[t];
    }
    // 获取从源点 s 到所有点的最短路径（包含路径记录）
    std::pair<std::vector<int>, std::vector<int>> ShortestPathWithRoute(int s)
    {
        std::vector<int>dist(n,INT_MAX);
        std::vector<int> prev(n,-1);
        dist[s] = 0;
        //优先队列，最小堆
        std::priority_queue<pii,std::vector<pii>,std::greater<pii>> pq;
        pq.push({0,s});//当前距离，当前节点
        
        while(!pq.empty())
        {
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if(d>dist[u]) continue;

            for(auto& edge:graph[u])
            {
                int v = edge.first;
                int w = edge.second;
                //松弛操作
                if(dist[u] + w < dist[v])
                {
                    dist[v] = dist[u]+w;
                    prev[v] = u;
                    pq.push({dist[v],v});
                }
            }
        }
        return {dist,prev};
    }
};