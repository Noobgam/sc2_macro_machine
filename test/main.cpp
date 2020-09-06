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

