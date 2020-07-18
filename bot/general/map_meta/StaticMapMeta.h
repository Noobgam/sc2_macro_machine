#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>
#include <vector>
#include <map>
#include <general/model/Common.h>
#include <general/DistanceMap.h>
#include <general/managers/resources/Resource.h>
#include <util/FileUtils.h>

#include <string>

class CCBot;

namespace boost::serialization {
    template<class Archive>
    void serialize(Archive& ar, CCPosition& pos, const unsigned int version) {
        ar & pos.x & pos.y;
    }
}

struct BaseLocationProjection {
    friend class boost::serialization::access;

    int baseId;
    CCPosition pos;
    CCPosition depotPos;

    int getBaseId() const {
        return baseId;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & baseId & pos & depotPos;
    }
};

struct StaticMapMeta {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar
            & m_width
            & m_height
            & m_walkable
            & m_buildable
            & m_terrainHeight
            & m_sectorNumber
            & m_unbuildableNeutral
            & m_unwalkableNeutral
            & m_baseLocationProjections;
    }

    StaticMapMeta();
    StaticMapMeta(const CCBot& bot);
    static std::unique_ptr<StaticMapMeta> getMeta(const CCBot& bot);
    static std::unique_ptr<StaticMapMeta> getMeta(std::string mapName);
    bool isValidTile(int x, int y) const;
    bool isValidTile(CCTilePosition tile) const;
    bool isBuildable(int tileX, int tileY) const;
    bool isWalkable(int tileX, int tileY) const;
    int getSectorNumber(int tileX, int tileY) const;
    float getTerrainHeight(float x, float y) const;

    // this is a heuristic, use MapTools::pylonPowers if Observation is available
    bool pylonPowers(const CCPosition& pylonPos, float radius, const CCPosition& candidate) const;

    void computeConnectivity();

    int width() const;
    int height() const;
    const std::vector<BaseLocationProjection>& getBaseLocations() const;
    DistanceMap getDistanceMap(const CCPosition& pos) const;

private:
    static std::vector<BaseLocationProjection> calculateBaseLocations(const CCBot& bot);
    static std::vector<std::vector<const Resource *>> findResourceClusters(const CCBot& bot);
    int     m_width;
    int     m_height;
    std::vector<std::vector<bool>>  m_walkable;           // whether a tile is buildable (does not include resources)
    std::vector<std::vector<bool>>  m_buildable;          // whether a tile is buildable (does not include resources)
    std::vector<std::vector<float>> m_terrainHeight;
    std::vector<std::vector<int>>   m_sectorNumber;       // sector number for maps which are not fully connected

    // initial locations of neutrals
    std::vector<std::vector<bool>>  m_unbuildableNeutral; // unbuildable rocks, plates, resources
    std::vector<std::vector<bool>>  m_unwalkableNeutral;  // unbuildable rocks, resources

    std::vector<BaseLocationProjection> m_baseLocationProjections;
};

BOOST_CLASS_VERSION(StaticMapMeta, 1)
BOOST_CLASS_TRACKING(StaticMapMeta, boost::serialization::track_never)


