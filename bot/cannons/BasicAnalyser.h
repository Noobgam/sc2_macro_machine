#pragma once

#include <vector>
#include <future>
#include <general/bases/BaseLocation.h>
#include "BaseAnalysis.h"

class CCBot;

class BasicAnalyser {
private:
public:
    int m_width;
    int m_height;
    std::vector<std::vector<bool>> statically_walkable;
    // keep in mind that in terms of analysis terran supply depos should not be considered unwalkable.
    // barracks probably can be.
    std::vector<std::vector<bool>> walkable;
    std::vector<std::vector<bool>> buildable;

    // geysers should be considered the same as walls, but minerals cannot be walked diagonally (except for pylon trick)
    std::vector<std::vector<bool>> minerals;

    std::atomic<BaseAnalysis*> latestAnalysis = NULL;
    std::atomic<int> analysisRevision = 0;
    std::future<void> lastCalculationFuture;

    // fields below are transient, only used in analysis directly

    // nexus position
    CCTilePosition tilePosition;

    // tiles that are candidates for pylon / cannon placements (left-bottom tile)
    std::vector<CCTilePosition> relevantTiles;

    // boolean relevance map, signifies whether
    std::vector<std::vector<bool>> isRelevantTile;

    BaseAnalysis* currentAnalysis = NULL;
    int currentPylonTarget;
    int currentCnt;
    std::vector<CCTilePosition> chosenPylons;

    void markUnbuildable(int x, int y, int size);
    void analyze(const BaseLocation* baseLocation);

    // recursion and helpers
    void checkCurrentPlacementAndAppend();
    bool addPylon(CCTilePosition tile);
    void removePylon(CCTilePosition tile);
    void recursion(const std::vector<CCTilePosition>& pylonCandidates);
    bool cutEarly() const;

public:
    void recalculate(const CCBot& bot);
    bool analysisReady();
    int getAnalysisRevision();

    void analyzeAsync(const BaseLocation* baseLocation);

};


