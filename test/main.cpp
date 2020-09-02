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

#include <signal.h>

void handler(int sig) {
    // unsure what to do in handler yet, but it might be useful.
}

int main(int argc, char* argv[])
{
    for (auto sig : {SIGABRT, SIGSEGV, SIGILL, SIGTERM}) {
        signal(sig, handler);
    }
    ::argc = argc;
    ::argv = argv;
    RunTestScenario(Scenarios::CAN_WIN_AGAINST_EMPTY_BOT);

    return 0;
}

