#pragma once
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <fstream>
#include <vector>
#include <map>

#include "util/FileUtils.h"
#include "WallPlacement.h"

class CCBot;

// A class that stores cached map metadata
// Could be anything: chokepoints, unit waypoints
// Can recalculate everything on the spot, but usually takes too long.
struct MapMeta {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & mapName & wallPlacements;
    }

    // this constructor is required by boost serialization
    MapMeta();
    MapMeta(const CCBot& bot);
    static std::unique_ptr<MapMeta> getMeta(const CCBot& bot);

    const std::vector<WallPlacement>& getWallPlacements(int startLocationId, int baseLocationId) const;
private:
    std::string mapName;
    // [startLocationId][locationId] -> wallPlacements
    // enemy location id is implied from startLocationId
    std::map<int, std::map<int, std::vector<WallPlacement>>> wallPlacements;
};

BOOST_CLASS_VERSION(MapMeta, 1)
BOOST_CLASS_TRACKING(MapMeta, boost::serialization::track_never)

