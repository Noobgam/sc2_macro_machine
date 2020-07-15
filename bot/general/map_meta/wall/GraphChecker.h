#pragma once

#include "GraphCheckResult.h"
#include <map>
#include <set>

struct GraphChecker {

    void addEdge(int x1, int y1, int x2, int y2);
    GraphCheckResult getResult(int sx, int sy, int tx, int ty);
private:
    int& getVertex(int x1, int y1);

    std::vector<std::pair<std::pair<int,int>, std::pair<int,int>>> initialEdges;

    std::vector<std::vector<std::pair<int, int>>> edges;
    std::map<int, std::map<int, int>> vertices;

    std::map<int, std::pair<int,int>> revEdges;
    std::map<int, std::pair<int,int>> revVertices;

    int vertexCount = 0;
    int edgeId = 0;

    // dfs and its state
    std::vector<bool> visited;
    std::vector<int> tin, low;
    std::set<int> cutpoints;
    std::set<int> bridges;
    int timer;

    void dfs(int v, int p = -1);

    std::vector<int> pts;
    std::vector<int> edgeIds;
    int t;
    bool done;

    void dfsTo(int v);

    void blindDfs(int v);

    // check whether source and result are connected after removal of v
    bool checkConnected(int v, int source, int result);
};


