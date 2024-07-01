#include "the5.h"

/* 
    in the5.h "struct Room" is defined as below:
    
    struct Room {
        int id;
        char content;
        vector<Room*> neighbors;
    };

*/

vector<int> dfsvisitor(Room* curr, stack<Room*> recentpath, vector<int>& path, int* visited, int roomc)
{
    vector<Room*> tneighbors = curr->neighbors;
    int neighborc = (curr->neighbors).size();
    int check = 0;
    if(recentpath.size() == 0)
    {
        return path;
    }
    if(neighborc == 0)
    {
        Room* pback = recentpath.top();
        recentpath.pop();
        dfsvisitor(pback, recentpath, path, visited, roomc);
    }
    if(curr->content == '*')
    {
        recentpath.pop();
        for(int i = 0; i<recentpath.size(); i++)
        {
            Room* curr;
            curr = recentpath.top();
            recentpath.pop();
            path.push_back(curr->id);
        }
    }
    for(int i = 0; i<neighborc; i++)
    {
        if(visited[tneighbors[i]->id] == 0 && tneighbors[i]->content == '*')
        {
            path.push_back(tneighbors[i]->id);
            int s = recentpath.size();
            for(int a = 0; a<roomc; a++)
            {
                visited[a] = 1;
            }
            while(s>0)
            {
                Room* curr;
                curr = recentpath.top();
                recentpath.pop();
                path.push_back(curr->id);
                s--;
            }
            return path;
        }
        else if(visited[tneighbors[i]->id] == 0)
        {
            check = 1;
            recentpath.push(tneighbors[i]);
            path.push_back(tneighbors[i]->id);
            visited[tneighbors[i]->id] = 1;
            dfsvisitor(tneighbors[i], recentpath, path, visited, roomc);
        }
    }
    
    if(check == 0)
    {
        recentpath.pop();
        Room* pback = recentpath.top();
        path.push_back(pback->id);
        dfsvisitor(pback, recentpath, path, visited, roomc);
    }
    return path;
}


vector<int> maze_trace(vector<Room*> maze) { 

    vector<int> path;
    //your code here
    std::stack<Room*> recentpath; //to keep track of visited nodes, path from room ptrs.
    int roomcount = maze.size(); //count of rooms in the maze.
    int visited[roomcount]; //if visited, id's index will be 1, ids start from 0.
    Room* mazeptr = maze.front(); //initialized to first elmt.
    vector<Room*> tneighbors = mazeptr->neighbors;
    int neighborc = (mazeptr->neighbors).size();
    int check = 0;
    
    //if node not visited --> trace neighbours
    //if no neighbours --> go back using path u have used to come there.
    for(int i = 0; i<roomcount; i++)
    {
        visited[i] = 0;
    }
    
    recentpath.push(mazeptr); //path'e ilk elemani ekledik.
    path.push_back(mazeptr->id);
    visited[mazeptr->id] = 1;
    
    if(mazeptr->id == '*')
    {
        return path;
    }
    
    path = dfsvisitor(mazeptr, recentpath, path, visited, roomcount);

    
    
    

    return path; // this is a dummy return value. YOU SHOULD CHANGE THIS!
}
