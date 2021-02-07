#include "general/CCBot.h"
#include "util/JSONTools.h"
#include "util/Util.h"
#include "util/FileUtils.h"

#include <memory>

#ifdef _DEBUG
int main(int argc, char* argv[])
{
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) 
    {
        std::cout << "Unable to find or parse settings." << std::endl;
        return 1;
    }
    
    std::string config = JSONTools::ReadFile("BotConfig.txt");
    if (config.length() == 0)
    {
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

    std::string botRaceString;
    std::string enemyRaceString;
    std::string mapString;
    int stepSize = 1;
    sc2::Difficulty enemyDifficulty = sc2::Difficulty::HardVeryHard;

    if (j.count("SC2API") && j["SC2API"].is_object())
    {
        const json & info = j["SC2API"];
        JSONTools::ReadString("BotRace", info, botRaceString);
        JSONTools::ReadString("EnemyRace", info, enemyRaceString);
        JSONTools::ReadString("MapFile", info, mapString);
        JSONTools::ReadInt("StepSize", info, stepSize);
        JSONTools::ReadInt("EnemyDifficulty", info, enemyDifficulty);
    }
    else
    {
        std::cerr << "Config file has no 'Game Info' object, required for starting the bot\n";
        std::cerr << "Please read the instructions and try again\n";
        exit(-1);
    }

    // Add the custom bot, it will control the players.
    CCBot bot;

    
    // WARNING: Bot logic has not been thorougly tested on step sizes > 1
    //          Setting this = N means the bot's onFrame gets called once every N frames
    //          The bot may crash or do unexpected things if its logic is not called every frame
    coordinator.SetStepSize(stepSize);
    coordinator.SetRealtime(true);
    coordinator.SetRandomSeed({12345});
//    coordinator.SetRawAffectsSelection(true);
//
//    sc2::FeatureLayerSettings settings;
//    coordinator.SetFeatureLayers(settings);

    coordinator.SetParticipants({
        sc2::CreateParticipant(Util::GetRaceFromString(botRaceString), &bot),
        sc2::CreateComputer(Util::GetRaceFromString(enemyRaceString), enemyDifficulty, sc2::AIBuild::Macro)
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(mapString);
        
    // Step forward the game simulation.
    while (coordinator.Update());

    return 0;
}
#else
#include "LadderInterface.h"

#ifdef _WIN32
// nothing we can do on win, since execinfo is missing
void handler(int sig) {};

#else
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig) {
    void *array[40];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 40);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

int main(int argc, char* argv[]) {
    std::unique_ptr<CCBot> bot = std::make_unique<CCBot>();
    for (auto sig : {SIGABRT, SIGSEGV, SIGILL, SIGTERM}) {
        signal(sig, handler);
    }
    RunBot(argc, argv, bot.get(), sc2::Race::Protoss);
    std::cerr << "Bot cycle finished" << std::endl;
    return 0;
}
#endif

