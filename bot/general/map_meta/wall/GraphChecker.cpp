#include "GraphChecker.h"
#include <cassert>
#include <util/LogInfo.h>

using std::min;

void GraphChecker::addEdge(int x1, int y1, int x2, int y2) {
    int l = getVertex(x1, y1);
    int r = getVertex(x2, y2);
    edgeId++;
    edges[l].push_back({r, edgeId});
    edges[r].push_back({l, edgeId});
    revEdges[edgeId] = {l, r};
}

GraphCheckResult GraphChecker::getResult(int sx, int sy, int tx, int ty) {
    LOG_DEBUG << "Getting result" << endl;

    int source = getVertex(sx, sy);
    int result = getVertex(tx, ty);

    // calculate all articulation points
    visited.assign(vertexCount, false);
    tin.assign(vertexCount, -1);
    low.assign(vertexCount, -1);
    dfs(source, -1);

    if (!visited[result]) {
        return {};
    }
    LOG_DEBUG << "dfs done" << endl;

    // select only ones on a path from s to t, all bridges and all articulation points
    //  MUST be on that path. Otherwise they aren't bridges/articulation points.
    visited.assign(vertexCount, false);
    t = result;
    done = false;
    dfsTo(source);
    LOG_DEBUG << "dfsTo done" << endl;

    GraphCheckResult res;

    for (auto x : pts) {
        if (cutpoints.count(x)) {
            res.articulationPoints.push_back(revVertices[x]);
        }
    }
    for (auto x : edgeIds) {
        if (bridges.count(x)) {
            auto lr = revEdges[x];
            res.bridges.push_back({revVertices[lr.first], revVertices[lr.second]});
        }
    }
    return res;
}

int &GraphChecker::getVertex(int x1, int y1) {
    int& val = vertices[x1][y1];
    if (val == 0) {
        val = ++vertexCount;
        revVertices[val] = {x1, y1};
    }
    return val;
}

void GraphChecker::dfs(int v, int p) {
    assert(v <= vertexCount);
    visited[v] = true;
    tin[v] = low[v] = timer++;
    int children=0;
    for (auto&& lr : edges[v]) {
        int to = lr.first;
        if (to == p) continue;
        if (visited[to]) {
            low[v] = min(low[v], tin[to]);
        } else {
            dfs(to, v);
            low[v] = min(low[v], low[to]);
            if (low[to] >= tin[v] && p!=-1) {
                cutpoints.insert(v);
            }
            if (low[to] > tin[v]) {
                bridges.insert(lr.second);
            }
            ++children;
        }
    }
    if(p == -1 && children > 1)
        cutpoints.insert(v);
}

void GraphChecker::dfsTo(int v) {
    visited[v] = true;
    if (v == t) {
        done = true;
        return;
    }
    pts.push_back(v);
    for (auto& lr : edges[v]) {
        if (visited[lr.first]) {
            continue;
        }
        edgeIds.push_back(lr.second);
        dfsTo(lr.first);
        if (done) return;
        edgeIds.pop_back();
    }
    pts.pop_back();
}
