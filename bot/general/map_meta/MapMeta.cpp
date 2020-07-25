#include "MapMeta.h"
#include "general/CCBot.h"
#include <memory>
#include <fstream>

#include "util/LogInfo.h"

using std::string;

MapMeta::MapMeta(const StaticMapMeta &meta) {
    auto&& locs = meta.getStartLocationIds();
    for (int i = 0; i < locs.size(); ++i) {
        int myStart = locs[i];
        scoutingKeyPoints.push_back(ScoutingKeyPoints::getScoutingKeyPoints(meta, myStart));

        for (int j = 0; j < locs.size(); ++j) {
            if (j == i) continue;
            int enemyStart = locs[j];
            auto&& vwp = WallPlacement::getWallsForBaseLocation(
                    meta,
                    myStart,
                    myStart,
                    enemyStart
            );
            wallPlacements.insert(wallPlacements.end(), vwp.begin(), vwp.end());
        }
    }
}

std::unique_ptr<MapMeta> MapMeta::getMeta(const CCBot &bot) {
    string mapName = bot.Observation()->GetGameInfo().map_name;
    string fileName = "data/map_metas/" + mapName;
    if (FileUtils::fileExists(fileName)) {
        std::unique_ptr<MapMeta> meta;
        std::ifstream ifs = FileUtils::openReadFile(fileName);
        boost::archive::text_iarchive ia(ifs);
        ia >> meta;
        LOG_DEBUG << "Successfully loaded map [" + mapName + "] from stash" << endl;
        return meta;
    } else {
        LOG_DEBUG << "Could not find a map [" + mapName + "] in stash, cannot continue, will take too long to recalc" << endl;
        std::terminate();
    }
}

std::unique_ptr<MapMeta> MapMeta::calculateMeta(const StaticMapMeta &meta, string mapName) {
    string fileName = "data/map_metas/" + mapName;
    LOG_DEBUG << "Started calculating [" + mapName + "] meta." << endl;
    auto ptr = std::make_unique<MapMeta>(meta);
    auto ofs = FileUtils::openWriteFile(fileName);
    boost::archive::text_oarchive oa(ofs);
    oa << ptr;
    LOG_DEBUG << "Finished calculating [" + mapName + "] meta." << endl;
    return ptr;
}

MapMeta::MapMeta() {}

std::vector<WallPlacement> MapMeta::getWallPlacements(int startLocationId, int baseLocationId) const {
    std::vector<WallPlacement> walls;
    for (auto& x : wallPlacements) {
        if (x.startLocationId == startLocationId && x.baseLocationId == baseLocationId) {
            walls.push_back(x);
        }
    }
    return walls;
}
