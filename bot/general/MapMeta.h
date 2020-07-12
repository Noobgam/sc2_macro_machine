#pragma once

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <fstream>

#include "util/FileUtils.h"

class CCBot;

// A class that stores cached map metadata
// Could be anything: chokepoints, unit waypoints
// Can recalculate everything on the spot, but usually takes too long.
struct MapMeta {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & mapName;
    }

    // this constructor is required by boost serialization
    MapMeta();
    MapMeta(const CCBot& bot);
    static std::unique_ptr<MapMeta> getMeta(const CCBot& bot);
private:
    std::string mapName;
};

BOOST_CLASS_VERSION(MapMeta, 0)


