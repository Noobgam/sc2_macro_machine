#pragma once

#include "../../../general/CCBot.h"
#include "WorkerBuildManager.h"
#include "GasBuildManager.h"

class EconomyBuildManager : public BuildManager {
private:
    CCBot& m_bot;
    WorkerBuildManager m_worker_manager;
    GasBuildManager    m_gas_manager;
public:
    EconomyBuildManager(CCBot& bot);
    virtual std::optional<BuildOrderItem> getTopPriority();
};
