#pragma once

#include "../../../general/CCBot.h"
#include "WorkerBuildManager.h"
#include "GasBuildManager.h"
#include "ExpandBuildManager.h"

class EconomyBuildManager : public BuildManager {
private:
    CCBot& m_bot;
    WorkerBuildManager  m_worker_manager;
    GasBuildManager     m_gas_manager;
    ExpandBuildManager  m_expand_manager;
public:
    EconomyBuildManager(CCBot& bot);
    virtual std::optional<BuildOrderItem> getTopPriority();
};
