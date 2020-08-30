#include "AbstractInfluenceMap.h"
#include <util/Util.h>

AbstractInfluenceMap::AbstractInfluenceMap(int width, int height)
        : map_width(width)
        , map_height(height)
        , enemyInfluence(width * height)
        , allyInfluence(width * height)
{}

void AbstractInfluenceMap::clear() {
    Util::setVectorZero(enemyInfluence);
    Util::setVectorZero(allyInfluence);
}

float AbstractInfluenceMap::getAllyInfluence(int x, int y) {
    if (x < 0 || x >= map_width || y < 0 || y >= map_height) {
        return 0;
    }
    return allyInfluence[map_width * y + x];
}

float AbstractInfluenceMap::getEnemyInfluence(int x, int y) {
    if (x < 0 || x >= map_width || y < 0 || y >= map_height) {
        return 0;
    }
    return enemyInfluence[map_width * y + x];
}
