#include <iostream>
#include <vector>

class UnionFind
{
private:
    std::vector<int> parent;
    std::vector<int> rank;
public:
    UnionFind(int n){
        parent.resize(n);
        rank.resize(n,1);
        for(int i = 0;i<n;i++)
        {
            parent[i] = i;
        }
    }

    int find(int x)
    {
        if(parent[x]==x)
        {
            return x;
        }
        else 
        {
            return parent[x] = find(parent[x]); 
        }
    }

    void unite(int x,int y)
    {
        int rootx = find(x);
        int rooty = find(y);
        if(rootx==rooty) return ;
        if(rank[rootx]<rank[rooty])
        {
            parent[rootx] = rooty;
        }
        else if(rank[rootx]>rank[rooty])
        {
            parent[rooty] = rootx;
        }
        else
        {
            parent[rooty] = rootx;
            rank[rootx]++;
        }
    }
    int getRank(int x)
    {
        return rank[find(x)];
    }
};