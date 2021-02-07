#include "ExactDistanceMap.h"

#include <queue>
#include <algorithm>

using std::vector;

const int D = 1;

inline int step(int x) {
    if (x < 0) {
        return 1;
    } else {
        return -1;
    }
}

inline int roundD(int x) {
    if (x < 0) {
        return -D;
    } else {
        return D;
    }
}

inline float dist(float a, float b) {
    return sqrt(a * a + b * b);
}

ExactDistanceMap::ExactDistanceMap(
        int width,
        int height,
        const CCTilePosition &startTile,
        int limit,
        std::function<int(int, int)> walkable
)   : m_width(width)
    , m_height(height)
    , m_startTile(startTile)
    , limit(limit)
    , walkable(walkable)
{
    std::priority_queue<pair<float, pair<int,int>>> pq;
    pq.push({0, {startTile.x, startTile.y}});
    auto relax = [this](int x, int y, float f) {
        auto&& it = m_dist.insert({{x, y}, f});
        if (it.second) {
            return true;
        } else {
            if (it.first->second > f) {
                it.first->second = f;
                return true;
            }
            return false;
        }
    };
    while (!pq.empty()) {
        auto lr = std::move(pq.top());
        pq.pop();
        int x = lr.second.first;
        int y = lr.second.second;
        float d = -lr.first;
        float realD = m_dist[{x, y}];
        if (realD != d) {
            continue;
        }
        if (d > limit) {
            break;
        }

        vector<vector<char>> blocked = std::vector<std::vector<char>>(2 * D + 1, std::vector<char>(2 * D + 1));
        // if there are walls around us, we'll have to be more careful
        // technically, this is not ideal, will be noticeable on maps with small columns

        for (int dx = -D; dx <= D; ++dx) {
            for (int dy = -D; dy <= D; ++dy) {
                int tox = dx + x;
                int toy = dy + y;
                if (!walkable(tox, toy)) {
                    blocked[dx + D][dy + D] = true;
                }
            }
        }
        for (int sx : {-1, 1}) {
            for (int sy : {-1, 1}) {
                int Dx = roundD(sx);
                int Dy = roundD(sy);
                for (int dx = 0; dx != Dx; dx += sx) {
                    bool nowBlocked = false;
                    for (int dy = 0; dy != Dy; dy += sy) {
                        int tox = dx + D;
                        int toy = dy + D;
                        nowBlocked |= blocked[tox][toy];
                        blocked[tox][toy] = nowBlocked;
                    }
                }
            }
        }
        for (int dx = -D; dx <= D; ++dx) {
            for (int dy = -D; dy <= D; ++dy) {
                if (!blocked[dx + D][dy + D]) {
                    int tox = dx + x;
                    int toy = dy + y;
                    float distance = dist(dx, dy) + d;
                    if (relax(tox, toy, distance)) {
                        pq.push({-distance, {tox, toy}});
                    }
                }
            }
        }
    }
}