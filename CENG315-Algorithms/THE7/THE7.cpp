#include "the7.h"

// Implement your solution after this line
/*struct Road {
  std::pair<int, int> buildings;
  int time;
  Road(std::pair<int, int> bs, int t) : buildings(bs), time(t) {}
};*/

int twovertexedge(int u, int v, std::vector<Road> roads)
{
    int siz = roads.size();
    int ind;
    for(int i = 0; i<siz; i++)
    {
        if(roads[i].buildings.first == u && roads[i].buildings.second == v)
        {
            ind = i;
        }
        else if(roads[i].buildings.first == v && roads[i].buildings.second == u)
        {
            ind = i;
        }
    }
    return ind;
}
int calculator(std::vector<int> path, std::vector<Road> roads)
{
    std::vector<int> temp = path;
    int result = 0;
    int ts = temp.size();
    for(int i = 0; i<ts-1; i++)
    {
        int cur = twovertexedge(temp[i], temp[i+1], roads);
        result+= roads[cur].time;
    }
    return result;
}
void generateInnerPath(int u, int v,int** fi, std::vector<int>* p)
{
    
    if(fi[u][v] != INT_MAX)
    {
        generateInnerPath(u, fi[u][v], fi, p);
        (*p).push_back(fi[u][v]);
        generateInnerPath(fi[u][v], v, fi, p);
    }
}
std::vector<int> generatePath(int u, int v, int** fi)
{
    std::vector<int> p;
    std::vector<int>* ptr;
    ptr = &p;
    p.push_back(u);
    generateInnerPath(u, v, fi, ptr);
    p.push_back(v);
    return p;
}

void CanCatch(int n, std::vector<Road> roads, int s, int d, int x, int y, int l, int printPath) {
    // You can change these variables. These are here for demo only.
    std::vector<int> path;
    std::vector<int> pthptr;
    int cost = INT_MAX;
    int rs = roads.size();
    int dst[n][n];
    int fi[n][n];
    int *px[n];  
    
    for(int i = 0; i<n; i++)
    {
        for(int j = 0; j<n; j++)
        {
            dst[i][j] = cost;
            fi[i][j] = cost;
        }
    }
    
    for(int i = 0; i<n; i++)
    {
        dst[i][i] = 0;
    }
    
    for(int i = 0; i<rs; i++)
    {
        int u = roads[i].buildings.first;
        int v = roads[i].buildings.second;
        
        dst[u][v] = roads[i].time;
        dst[v][u] = roads[i].time;
    }
    for(int k = 0; k<n; k++)
    {
        for(int i = 0; i<n ; i++)
        {
            for(int j = 0; j<n; j++)
            {
                if((dst[i][j]>(dst[i][k]+dst[j][k])) && (dst[k][j] != INT_MAX && dst[i][k] !=INT_MAX))
                {
                    dst[i][j] = dst[i][k]+dst[j][k];
                    fi[i][j] = k;
                }
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        px[i] = fi[i];
    }
    std::vector<int> visitedX = generatePath(s,x, px);
    std::vector<int> visitedXY = generatePath(x,y, px);
    
    std::vector<int> visitedYD = generatePath(y,d, px);
    std::vector<int> visitedXD = generatePath(x,d, px);
    
    std::vector<int> visitedY = generatePath(s,y, px);
    std::vector<int> visitedYX = generatePath(y,x, px);
    
    std::vector<int> visitedD = generatePath(s,d, px);
    
    int vX = calculator(visitedX, roads);
    int vXY = calculator(visitedXY, roads);
    int vYD = calculator(visitedYD, roads);
    int vXD = calculator(visitedXD, roads);
    int vY = calculator(visitedY, roads);
    int vYX = calculator(visitedYX, roads);
    int vD = calculator(visitedD, roads);
    
    int XYD = vX+vXY+vYD;
    int XD = vX+vXD;
    int D = vD;
    int YD = vY+vYD;
    int YXD = vY+ vYX+vXD;
    
    if(D>l)
    {
        std::cout<<"IMPOSSIBLE"<<std::endl;
        return;
    }
    else if(XYD<=l && XYD<=YXD)
    {
        std::cout<<"YES BOTH "<<XYD<<"MINUTES"<<std::endl;
        path = visitedX;
        path.pop_back();
        path.insert(path.end(), visitedXY.begin(), visitedXY.end());
        path.pop_back();
        path.insert(path.end(), visitedYD.begin(), visitedYD.end());
        if(printPath == 1)
        {
            PrintRange(path.begin(), path.end());
        }
        return;
    }
    else if(YXD<=l)
    {
        std::cout<<"YES BOTH "<<YXD<<"MINUTES"<<std::endl;
        path = visitedY;
        path.pop_back();
        path.insert(path.end(), visitedYX.begin(), visitedYX.end());
        path.pop_back();
        path.insert(path.end(), visitedXD.begin(), visitedXD.end());
        if(printPath == 1)
        {
            PrintRange(path.begin(), path.end());
        }
        return;
    }
    else if(XD<=l)
    {
        std::cout<<"YES PRINTER "<<XD<<"MINUTES"<<std::endl;
        path = visitedX;
        path.pop_back();
        path.insert(path.end(), visitedXD.begin(), visitedXD.end());
        if(printPath == 1)
        {
            PrintRange(path.begin(), path.end());
        }
        return;
    }
    else if(YD<=l)
    {
        std::cout<<"YES DORM "<<YD<<"MINUTES"<<std::endl;
        path = visitedY;
        path.pop_back();
        path.insert(path.end(), visitedYD.begin(), visitedYD.end());
        if(printPath == 1)
        {
            PrintRange(path.begin(), path.end());
        }
        return;
    }
    else
    {
        std::cout<<"YES DIRECTLY "<<D<<"MINUTES"<<std::endl;
        path = visitedD;
        if(printPath == 1)
        {
            PrintRange(path.begin(), path.end());
        }
        return;
    }
    
