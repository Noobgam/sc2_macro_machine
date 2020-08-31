#include "AbstractInfluenceMap.h"
#include <util/Util.h>

void AbstractInfluenceMap::initialize(int width, int height) {
    map_width = width;
    map_height = height;
    enemyInfluence = std::vector<float>(width * height);
    allyInfluence = std::vector<float>(width * height);
}

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

float& AbstractInfluenceMap::getAllyInfluenceRef(int x, int y) {
    return allyInfluence[map_width * y + x];
}

float& AbstractInfluenceMap::getEnemyInfluenceRef(int x, int y) {
    return enemyInfluence[map_width * y + x];
}
