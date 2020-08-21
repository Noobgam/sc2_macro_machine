#include "general/CCBot.h"
#include "util/JSONTools.h"
#include "util/Util.h"
#include "util/FileUtils.h"
#include "common/TestScenario.h"
#include "scenarios/Scenarios.h"
#include "common/TestRunner.h"

#include <memory>

std::vector<TestScenario> TEST_SCENARIOS = {
    Scenarios::CAN_WIN_AGAINST_EMPTY_BOT
};

static int argc;
static char** argv;

void RunTestScenario(const TestScenario& scenario) {
    TestRunner runner{scenario};
    runner.run(argc, argv);
}

int main(int argc, char* argv[])
{
    ::argc = argc;
    ::argv = argv;
    RunTestScenario(Scenarios::CAN_WIN_AGAINST_EMPTY_BOT);

    return 0;
}

