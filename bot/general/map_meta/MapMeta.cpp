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
    wallPlacements[myStart][myStart] = std::move(vwp);
    vwp = WallPlacement::getWallsForBaseLocation(
            bot,
            enemyStart,
            enemyStart,
            myStart
    );
    wallPlacements[enemyStart][enemyStart] = std::move(vwp);
}

std::unique_ptr<MapMeta> MapMeta::getMeta(const CCBot &bot) {
    string mapName = bot.Observation()->GetGameInfo().map_name;
    string fileName = "data/map_metas/" + mapName;
    if (FileUtils::fileExists(fileName)) {
        std::unique_ptr<MapMeta> meta;
        std::ifstream ifs = FileUtils::openReadFile(fileName);
        boost::archive::binary_iarchive ia(ifs);
        ia >> meta;
        LOG_DEBUG << "Successfully loaded map [" + mapName + "] from stash" << endl;
        return meta;
    } else {
        LOG_DEBUG << "Could not find a map [" + mapName + "] in stash, will recalculate" << endl;
        auto ptr = std::make_unique<MapMeta>(bot);
        auto ofs = FileUtils::openWriteFile(fileName);
        boost::archive::binary_oarchive oa(ofs);
        oa << ptr;
        return ptr;
    }
}

MapMeta::MapMeta() {}

const std::vector<WallPlacement> &MapMeta::getWallPlacements(int startLocationId, int baseLocationId) const {
    return wallPlacements.at(startLocationId).at(baseLocationId);
}
