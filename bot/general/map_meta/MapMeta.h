#pragma once
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "util/FileUtils.h"
#include "general/map_meta/wall/WallPlacement.h"

class CCBot;

// A class that stores cached map metadata
// Could be anything: chokepoints, unit waypoints
// Can recalculate everything on the spot, but usually takes too long.
struct MapMeta {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & wallPlacements;
    }

    // this constructor is required by boost serialization
    MapMeta();
    MapMeta(const CCBot& bot);
    MapMeta(const StaticMapMeta& bot);
    static std::unique_ptr<MapMeta> getMeta(const CCBot& bot);
    static std::unique_ptr<MapMeta> getMeta(const StaticMapMeta& meta, std::string mapName);

    std::vector<WallPlacement> getWallPlacements(int startLocationId, int baseLocationId) const;
private:
    // [startLocationId][locationId] -> wallPlacements
    // enemy location id is implied from startLocationId
    std::vector<WallPlacement> wallPlacements;
};

BOOST_CLASS_VERSION(MapMeta, 1)
BOOST_CLASS_TRACKING(MapMeta, boost::serialization::track_never)

