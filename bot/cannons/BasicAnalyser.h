#pragma once

#include <vector>
#include <future>
#include <general/bases/BaseLocation.h>
#include "BaseAnalysis.h"

class CCBot;

class BasicAnalyser {
private:
    struct SlowCheckResult {
        std::vector<CCTilePosition> cannonPlacements;

        bool isSuccess() {
            return !cannonPlacements.empty()
                // disallow ramp blocks, they can be wrongly directed for some reason
                && cannonPlacements.size() < 10;
        }
    };

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
    std::atomic<bool> cancelRequested = false;
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
    bool fastCheck() const;
    bool canWalk(int fromx, int fromy, int tox, int toy) const;

    int visitedComp;
    std::vector<std::vector<int>> visitedSlow;
    // <size_of_component, whether it has non-relevant neighbouring cells>
    std::pair<int, bool> dfs(int x, int y);
    std::vector<CCTilePosition> dfsCannonPlacement(int x, int y, int comp);
    SlowCheckResult slowCheck();
    void checkCurrentPlacementAndAppend();
    bool addPylon(CCTilePosition tile);
    void removePylon(CCTilePosition tile);
    void recursion0(const std::vector<CCTilePosition>& pylonCandidates);
    void recursion(const std::vector<CCTilePosition>& pylonCandidates);
    bool cutEarly() const;
    void requestCancel();

public:
    bool recalculate(const CCBot& bot);
    bool analysisReady();
    int getAnalysisRevision();

    void analyzeAsync(const BaseLocation* baseLocation);

};


