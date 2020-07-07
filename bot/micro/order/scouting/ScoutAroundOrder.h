#pragma once
#include "../Order.h"
#include "../../../general/ExactDistanceMap.h"

// Scouts everything around a point
class ScoutAroundOrder : public Order {
private:
    CCTilePosition m_target_position;
    ExactDistanceMap* emp;
    std::vector<CCTilePosition> m_keyPoints;

    std::vector <CCTilePosition> orderTilesPerfectly(
            CCTilePosition start, std::vector<CCTilePosition> tilesToVisit
    );

    std::string getFileName();

    void saveKeyPointsToFile(const std::vector<std::pair<CCTilePosition, std::vector<CCTilePosition>>>& keypoints);
    std::vector<std::pair<CCTilePosition, std::vector<CCTilePosition>>> loadKeyPointsFromFile();
    std::vector<CCTilePosition> computeKeyPoints();

public:
    ScoutAroundOrder(CCBot & bot, Squad* squad, CCTilePosition position);
    ScoutAroundOrder(CCBot & bot, Squad* squad, CCPosition position);

    void onStart();

    void onStep();
};


