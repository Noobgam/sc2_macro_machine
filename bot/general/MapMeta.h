#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// A class that stores cached map metadata
// Could be anything: chokepoints, unit waypoints
// Can recalculate everything on the spot, but usually takes too long.
struct MapMeta {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & someInt;
    }

    int someInt;
};

BOOST_CLASS_VERSION(MapMeta, 0)


