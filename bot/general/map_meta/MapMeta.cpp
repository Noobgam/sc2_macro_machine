#include "MapMeta.h"
#include "general/CCBot.h"
#include <memory>
#include <fstream>

#include "util/LogInfo.h"

using std::string;

MapMeta::MapMeta(const CCBot &bot)
    : mapName(bot.Observation()->GetGameInfo().map_name)
{
    int myStart = bot.Bases().getPlayerStartLocation(Players::Self)->getBaseId();
    int enemyStart = bot.Bases().getPlayerStartLocation(Players::Enemy)->getBaseId();
    auto&& vwp = WallPlacement::getWallsForBaseLocation(
            bot,
            myStart,
            myStart,
            enemyStart
    );
    for (auto& x : vwp) {
        wallPlacements.push_back(x);
    }
    vwp = WallPlacement::getWallsForBaseLocation(
            bot,
            enemyStart,
            enemyStart,
            myStart
    );
    for (auto& x : vwp) {
        wallPlacements.push_back(x);
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
