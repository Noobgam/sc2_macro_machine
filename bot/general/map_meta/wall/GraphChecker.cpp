#include "GraphChecker.h"
#include <cassert>
#include <util/LogInfo.h>
#include <algorithm>

using std::min;

void GraphChecker::addEdge(int x1, int y1, int x2, int y2) {
    // getVertex has side effect of creating new vertex
    getVertex(x1, y1);
    getVertex(x2, y2);
    initialEdges.push_back({{x1, y1}, {x2, y2}});
}

namespace {

    struct cmp {
        bool operator() (const std::pair<int,int>& lhs, const std::pair<int,int>& rhs) const {
            return lhs.first < rhs.first;
        }
    };

    struct cmpeq {
        bool operator() (const std::pair<int,int>& lhs, const std::pair<int,int>& rhs) const {
            return lhs.first == rhs.first;
        }
    };
}

GraphCheckResult GraphChecker::getResult(int sx, int sy, int tx, int ty) {
    //LOG_DEBUG << "Getting result" << BOT_ENDL;

    int source = getVertex(sx, sy);
    int result = getVertex(tx, ty);

    edges.resize(vertexCount + 1);
    // calculate all articulation points
    visited.assign(vertexCount + 1, false);
    tin.assign(vertexCount + 1, -1);
    low.assign(vertexCount + 1, -1);
    timer = 0;

    for (auto x : initialEdges) {
        int x1 = x.first.first;
        int y1 = x.first.second;
        int x2 = x.second.first;
        int y2 = x.second.second;

        int l = getVertex(x1, y1);
        int r = getVertex(x2, y2);
        edgeId++;
        edges[l].push_back({r, edgeId});
        edges[r].push_back({l, edgeId});
    }
    for (int i = 1; i <= vertexCount; ++i) {
        std::sort(edges[i].begin(), edges[i].end(), cmp{});
        edges[i].erase(
                std::unique(edges[i].begin(), edges[i].end(), cmpeq{}),
                edges[i].end()
                );
    }
    dfs(source, -1);

    if (!visited[result]) {
        return {};
    }
    //LOG_DEBUG << "dfs done" << BOT_ENDL;

    // select only ones on a path from s to t, all bridges and all articulation points
    //  MUST be on that path. Otherwise they aren't bridges/articulation points.
    visited.assign(vertexCount + 1, false);
    t = result;
    done = false;
    dfsTo(source);
    //LOG_DEBUG << "dfsTo done" << BOT_ENDL;

    GraphCheckResult res;

    std::vector<int> arts;
    for (auto x : pts) {
        if (cutpoints.count(x)) {
            arts.push_back(x);
        }
    }

    for (auto x : arts) {
        if (!checkConnected(x, source, result)) {
            res.articulationPoints.push_back(revVertices[x]);
        }
    }
    for (auto x : edgeIds) {
        if (bridges.count(x)) {
            auto lr = revEdges[x];
            int dx = abs(revVertices[lr.first].first - revVertices[lr.second].first);
            int dy = abs(revVertices[lr.first].second - revVertices[lr.second].second);
            if (dx + dy == 2) {
                // only diagonal edges can be bridges.
                res.bridges.push_back({revVertices[lr.first], revVertices[lr.second]});
            }
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
    auto&& curv = revVertices[v];
    visited[v] = true;
    pts.push_back(v);
    if (v == t) {
        done = true;
        return;
    }
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

bool GraphChecker::checkConnected(int v, int source, int result) {

    visited.assign(vertexCount + 1, false);
    visited[v] = true;
    blindDfs(source);
    return visited[result];
}

void GraphChecker::blindDfs(int v) {
    visited[v] = true;
    for (auto& lr : edges[v]) {
        if (visited[lr.first]) {
            continue;
        }
        blindDfs(lr.first);
    }
}
