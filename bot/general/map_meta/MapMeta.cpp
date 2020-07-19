#include "MapMeta.h"
#include "general/CCBot.h"
#include <memory>
#include <fstream>

#include "util/LogInfo.h"

using std::string;

MapMeta::MapMeta(const CCBot &bot) {
    int myStart = bot.Bases().getPlayerStartLocation(Players::Self)->getBaseId();
    int enemyStart = bot.Bases().getPlayerStartLocation(Players::Enemy)->getBaseId();
    auto&& vwp = WallPlacement::getWallsForBaseLocation(
            bot.Map().getStaticMapMeta(),
            myStart,
            myStart,
            enemyStart
    );
    for (auto& x : vwp) {
        wallPlacements.push_back(x);
    }
    vwp = WallPlacement::getWallsForBaseLocation(
            bot.Map().getStaticMapMeta(),
            enemyStart,
            enemyStart,
            myStart
    );
    for (auto& x : vwp) {
        wallPlacements.push_back(x);
    }
}

MapMeta::MapMeta(const StaticMapMeta &meta) {
    auto&& locs = meta.getStartLocationIds();
    for (int i = 0; i < locs.size(); ++i) {
        for (int j = 0; j < locs.size(); ++j) {
            if (j == i) continue;
            int myStart = locs[i];
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
        LOG_DEBUG << "Could not find a map [" + mapName + "] in stash, will recalculate" << endl;
        auto ptr = std::make_unique<MapMeta>(bot);
        auto ofs = FileUtils::openWriteFile(fileName);
        boost::archive::text_oarchive oa(ofs);
        oa << ptr;
        return ptr;
    }
}

std::unique_ptr<MapMeta> MapMeta::getMeta(const StaticMapMeta &meta, string mapName) {
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
