#include <general/map_meta/StaticMapMeta.h>
#include <general/map_meta/MapMeta.h>
#include <general/map_meta/wall/WallPlacement.h>
#include <general/CCBot.h>

#include <memory>
#include <string>
#include <thread>
#include <util/JSONTools.h>
#include <util/Util.h>

using namespace std;

vector<string> ALL_MAPS = {
        //"DeathAuraLE.SC2Map",
        //"EternalEmpireLE.SC2Map",
        //"EverDreamLE.SC2Map",
        //"GoldenWallLE.SC2Map",
        "IceandChromeLE.SC2Map",
//        "PillarsofGoldLE.SC2Map",
//        "SubmarineLE.SC2Map"
};

// requires static meta to be already calculated
int main(int argc, char* argv[]) {
    for (auto mapString : ALL_MAPS) {
        sc2::Coordinator coordinator;
        if (!coordinator.LoadSettings(argc, argv)) {
            std::cout << "Unable to find or parse settings." << std::endl;
            return 1;
        }

        std::string config = JSONTools::ReadFile("BotConfig.txt");
        if (config.length() == 0) {
            std::cerr << "Config file could not be found, and is required for starting the bot\n";
            std::cerr << "Please read the instructions and try again\n";
            exit(-1);
        }

        std::ifstream file("BotConfig.txt");
        json j;
        file >> j;

        /*if (parsingFailed)
        {
            std::cerr << "Config file could not be parsed, and is required for starting the bot\n";
            std::cerr << "Please read the instructions and try again\n";
            exit(-1);
        }*/

        std::string botRaceString = "Protoss";
        std::string enemyRaceString = "Protoss";
        sc2::Difficulty enemyDifficulty = sc2::Difficulty::Hard;

        // Add the custom bot, it will control the players.
        CCBot bot;
        coordinator.SetStepSize(1);
        coordinator.SetRealtime(true);

        coordinator.SetParticipants({
                                            sc2::CreateParticipant(Util::GetRaceFromString(botRaceString), &bot),
                                            sc2::CreateComputer(Util::GetRaceFromString(enemyRaceString),
                                                                enemyDifficulty)
                                    });

        // Start the game.
        coordinator.LaunchStarcraft();
        coordinator.StartGame(mapString);

        // Step forward the game simulation.
        do {
            coordinator.Update();
        } while (bot.getObservationId() < 2);
    }

    return 0;
}