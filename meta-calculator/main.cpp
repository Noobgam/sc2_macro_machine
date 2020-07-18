#include <general/map_meta/StaticMapMeta.h>
#include <general/map_meta/MapMeta.h>

#include <memory>
#include <string>

using namespace std;

// requires static meta to be already calculated
int main(int argc, char* argv[]) {
    std::vector<string> maps;
    for (int i = 1; i < argc; ++i) {
        maps.push_back(string(argv[i]));
    }
    for (auto x : maps) {
        std::unique_ptr<StaticMapMeta> staticMapMeta = StaticMapMeta::getMeta(x);

    }
}