#pragma once

#include <vector>
#include <general/model/Unit.h>
#include <general/ObservedTech.h>

class AbstractInfluenceMap {
protected:
    // compact 2d vector
    std::vector<float> enemyInfluence;
    std::vector<float> allyInfluence;
    const int map_width;
    const int map_height;
    AbstractInfluenceMap(int width, int height);
public:
    void clear();
    float getAllyInfluence(int x, int y);
    float getEnemyInfluence(int x, int y);

    // XXX: is this a good interface?
    virtual void addInfluence(
            const sc2::UnitTypeData& typeData,
            CCPosition position,
            CCPlayer player,
            ObservedTech tech
    ) = 0;
};