#include <iostream>
#include <vector>
#include <queue>
#include <mcpp/mcpp.h>
mcpp::MinecraftConnection mc;

struct coord {int x, y, z;
};

//Function to reverse the array to show from start to end
std::vector<coord> reverse(std::vector<coord> list) {
    std::vector<coord> answer;
    for (int i = static_cast<int>(list.size() - 1); i >= 0; i--) {
        answer.push_back(list[i]);
    }
    return answer;
}

//A breadth first search function that searches for the shortest path of two coordinates. only partially functional rn
std::vector<coord> bfs(coord start, coord end) {
    std::queue<coord> q;  //this queue will be replaced in place for an array 
    bool visited[50][50] = {{false}}; //boolean array thats stores what coordinate has been visited
    coord parent[50][50][50]; //array that is used to track pathing, allow for selection of closes path

    q.push(start);
    visited[start.x][start.z] = true;
    parent[start.x][start.y][start.z] = start;

    //int arrays that will be used to move the coordinate
    int DX[] = {1, -1, 0, 0};
    int DZ[] = {0, 0, 1, -1};

    
    while (!q.empty()) {
        coord current = q.front();
        q.pop();
        if (current.x == end.x && current.z == end.z) {
            break;
        }
        for (int i = 0; i < 4; ++i) {
            int nx = current.x + DX[i];
            int nz = current.z + DZ[i];
            int ny = current.y;
            if (nx < 0 || nz < 0 || visited[nx][nz] == true) {continue;}; //conditions that will be checked, not yet finished
            parent[nx][ny][nz] = current; //stores the coordinate in the parent array if the conditions passes
            visited[nx][nz] = true; 
            coord newCoord{nx, ny, nz};
            q.push(newCoord);
        }
    }

    std::vector<coord> result;
    coord temp = end; // 2 1
    result.push_back(end); // 2 1


    while (!(temp.x == start.x && temp.z == start.z)) {
        result.push_back(parent[temp.x][temp.y][temp.z]); //push the parent of each coordinate to the array
        temp = parent[temp.x][temp.y][temp.z]; //make temp be the parent of the previous array, this will repeat till the start is found
    }

    result = reverse(result); //reverse the result since its the end to start
    return result;
};

int main() {

    coord start{10, mc.getHeight(mcpp::Coordinate2D(10,1)), 1};
    coord end{25, mc.getHeight(mcpp::Coordinate2D(25,6)), 6};
    std::vector<coord> result = bfs(start, end);
    


    //testing for laying down block, using dirt rn cause they dont fall, will be replace later
    for (size_t i = 0; i < result.size(); ++i) {
        mcpp::Coordinate blockTile(result[i].x, result[i].y, result[i].z);
        mc.setBlock(blockTile, mcpp::Blocks::DIRT);
    }

    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << "{ " << result[i].x  << ", " << result[i].y << ", " << result[i].z  << " }" << std::endl; //testing purposes
    }
}
