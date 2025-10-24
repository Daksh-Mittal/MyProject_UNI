#include "path.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include "models/Node/queue.h"

// gets the surface height, keeps going down if block is leaf/wood to avoid trees, note to self: (remove if trees are cleared during terrain generation)
static inline int surfaceYStand(mcpp::MinecraftConnection& mc, int x, int z) {
    int y = mc.getHeight(mcpp::Coordinate2D(x, z));
    auto b = mc.getBlock({x, y, z});
    //checks if the current block is on a tree
    while (b == mcpp::Blocks::OAK_LEAVES || b == mcpp::Blocks::SPRUCE_LEAVES || b == mcpp::Blocks::BIRCH_LEAVES || b == mcpp::Blocks::JUNGLE_LEAVES || b == mcpp::Blocks::ACACIA_LEAVES || b == mcpp::Blocks::OAK_WOOD|| b == mcpp::Blocks::SPRUCE_WOOD || b == mcpp::Blocks::BIRCH_WOOD || b == mcpp::Blocks::JUNGLE_WOOD || b == mcpp::Blocks::ACACIA_WOOD) {
        --y;
        b = mc.getBlock({x, y, z});
    }
    return y + 1;
}

//allocate visited array for bfs
bool** createVisited(int l, int w) {
    bool** visited = new bool*[l];
    for (int i = 0; i < l; ++i)
        visited[i] = new bool[w]();
    return visited;
}

//function to delete visited array
void destroyVisited(bool** visited, int l) {
    for (int i = 0; i < l; ++i)
        delete[] visited[i];
    delete[] visited;
}

// allocates parent array used to backtrack the path
mcpp::Coordinate** createPathArray(int l, int w) {
    mcpp::Coordinate** parent = new mcpp::Coordinate*[l];
    for (int i = 0; i < l; ++i)
        parent[i] = new mcpp::Coordinate[w];
    return parent;
}

//function to delete the paretn array once done
void deleteParent(mcpp::Coordinate** parent, int l) {
    for (int i = 0; i < l; ++i)
        delete[] parent[i];
    delete[] parent;
}

Path::Path(mcpp::Coordinate start, mcpp::Coordinate end) :start(start), end(end) {}

//BFS algorithm to find the shortest walkable path between two points
void breadth_first_search(Path& newPath, mcpp::MinecraftConnection& mc) {
    const int pad = 4;  //how far the algo will scan around path
    const int minX = std::min(newPath.start.x, newPath.end.x) - pad;
    const int minZ = std::min(newPath.start.z, newPath.end.z) - pad;
    const int L = std::abs(newPath.start.x - newPath.end.x) + 2*pad + 1;
    const int W = std::abs(newPath.start.z - newPath.end.z) + 2*pad + 1;

    //lambda shortcuts
    auto ix = [&](int x){return x - minX;};
    auto iz = [&](int z){return z - minZ;};
    auto inb = [&](int x, int z){return x >= minX && z >= minZ && x < minX + L && z < minZ + W;};

    //store heights and mark which spots is solid ground
    int** H = new int*[L];
    std::vector<std::vector<bool>> solid(L, std::vector<bool>(W, false));

    for (int i = 0;i < L; ++i){
        H[i] = new int[W];
        for (int j=0; j<W; ++j){
            int x = minX + i, z = minZ + j;
            H[i][j] = surfaceYStand(mc, x, z);
            auto under = mc.getBlock({x, H[i][j] - 1, z});
            solid[i][j] = !(under==mcpp::Blocks::AIR || under == mcpp::Blocks::STILL_WATER);
        }
    }

    //set start and end y positions
    newPath.start.y = H[ix(newPath.start.x)][iz(newPath.start.z)];
    newPath.end.y = H[ix(newPath.end.x)][iz(newPath.end.z)];

    bool** visited = createVisited(L,W);
    mcpp::Coordinate** parent = createPathArray(L,W);
    visited[ix(newPath.start.x)][iz(newPath.start.z)] = true;
    parent[ix(newPath.start.x)][iz(newPath.start.z)] = newPath.start;

    Queue q;
    q.push(newPath.start);

    //4 directional BFS
    const int DX[4] = {1,-1,0,0};
    const int DZ[4] = {0,0,1,-1};

    bool found = false;
    while(!q.isEmpty()){
        mcpp::Coordinate cur = q.getCurrent();
        q.popFront();

        if(cur.x == newPath.end.x && cur.z == newPath.end.z){
            found = true; break;
        }

        for(int k = 0; k < 4; ++k){
            int nx = cur.x+DX[k], nz = cur.z + DZ[k];
            if(!inb(nx,nz)) continue; //out of bound
            int I = ix(nx), J = iz(nz);
            if(visited[I][J]) continue; //checks if we visited the coord already

            int nY = H[I][J];
            int cY = cur.y;

            
            if(std::abs(cY - nY) > 1) continue;
            if(!solid[I][J]) continue;

            visited[I][J] = true;
            parent[I][J] = cur;
            q.push({nx,nY,nz});
        }
    }

    //build actual path
    if(found){
        std::vector<mcpp::Coordinate> pathNodes;
        mcpp::Coordinate t = newPath.end;

        //back track the path
        while(!(t.x == newPath.start.x && t.z == newPath.start.z)){
            pathNodes.push_back(t);
            t=parent[ix(t.x)][iz(t.z)];
        }
        pathNodes.push_back(newPath.start);

        //place path
        for(auto& c: pathNodes){
            int sy = H[ix(c.x)][iz(c.z)];
            mc.setBlock({c.x, sy - 1, c.z}, mcpp::Blocks::GRAVEL); //places gravel as path

            //place support block under if under is water or air
            for(int y = sy - 2; y >= 0; --y){
                auto b = mc.getBlock({c.x,y,c.z});
                if(b!=mcpp::Blocks::AIR && b!=mcpp::Blocks::STILL_WATER) {
                    break;
                }
                mc.setBlock({c.x,y,c.z}, mcpp::Blocks::STONE);
            }
        }
    }

    //memory cleanup
    for (int i = 0; i < L; ++i) delete[] H[i];
    delete[] H;
    deleteParent(parent,L);
    destroyVisited(visited,L);
}
