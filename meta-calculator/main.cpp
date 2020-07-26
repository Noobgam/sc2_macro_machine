#include <general/map_meta/StaticMapMeta.h>
#include <general/map_meta/MapMeta.h>
#include <general/map_meta/wall/WallPlacement.h>

#include <memory>
#include <string>
#include <thread>

using namespace std;

// requires static meta to be already calculated
int main(int argc, char* argv[]) {
    std::vector<string> maps;
    for (int i = 1; i < argc; ++i) {
        maps.push_back(string(argv[i]));
    }
    setVerifierThreadCount(std::thread::hardware_concurrency());
    for (auto mapName : maps) {
        std::unique_ptr<StaticMapMeta> staticMapMeta = StaticMapMeta::getMeta(mapName);
        std::unique_ptr<MapMeta> meta = MapMeta::calculateMeta(*staticMapMeta, mapName);
    }
    return 0;
}