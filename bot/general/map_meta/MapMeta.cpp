#include "MapMeta.h"
#include "general/CCBot.h"
#include <memory>
#include <fstream>

#include "util/LogInfo.h"

using std::string;

MapMeta::MapMeta(const StaticMapMeta &meta) {
    auto&& locs = meta.getStartLocationIds();
    for (int i = 0; i < locs.size(); ++i) {
        int enemyStart = locs[i];
        for (const auto &lr : meta.getOrderedBasesByStartLocationId()) {
            int myStart = lr.first;
            if (myStart == enemyStart) continue;
            scoutingKeyPoints.push_back(ScoutingKeyPoints::getScoutingKeyPoints(meta, myStart));

            for (int baseIndex = 0; baseIndex < 2 && baseIndex < lr.second.size(); ++baseIndex) {
                int myBase = lr.second[baseIndex];
                // this would be dumb anyway.
                if (myBase == enemyStart) continue;
                auto &&vwp = WallPlacement::getWallsForBaseLocation(
                        meta,
                        myBase,
                        myStart,
                        enemyStart
                );
                wallPlacements.insert(wallPlacements.end(), vwp.begin(), vwp.end());
            }
        }
    }
}

std::unique_ptr<MapMeta> MapMeta::getMeta(string mapName) {
    string fileName = "data/map_metas/" + mapName;
    if (FileUtils::fileExists(fileName)) {
        std::unique_ptr<MapMeta> meta;
        meta = std::make_unique<MapMeta>();
        std::ifstream ifs = FileUtils::openReadFile(fileName);
        boost::archive::text_iarchive ia(ifs);
        ia >> meta;
        LOG_DEBUG << "Successfully loaded map [" + mapName + "] from stash" << endl;
        return meta;
    } else {
        LOG_DEBUG << "Could not find a map [" + mapName + "] in stash, loading empty map meta instead" << endl;
        return std::make_unique<MapMeta>();
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
