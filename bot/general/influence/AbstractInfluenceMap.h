#pragma once

#include <vector>
#include <general/model/Unit.h>
#include <general/ObservedTech.h>

class AbstractInfluenceMap {
protected:
    // compact 2d vector
    std::vector<float> enemyInfluence;
    std::vector<float> allyInfluence;
    int map_width;
    int map_height;
    float& getAllyInfluenceRef(int x, int y);
    float& getEnemyInfluenceRef(int x, int y);
public:
    void clear();
    void initialize(int width, int height);
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