#pragma once

#include "WorkerBuildManager.h"
#include "GasBuildManager.h"
#include "ExpandBuildManager.h"

class CCBot;

class EconomyBuildManager : public BuildManager {
private:
    WorkerBuildManager  m_worker_manager;
    GasBuildManager     m_gas_manager;
    ExpandBuildManager  m_expand_manager;
public:
    explicit EconomyBuildManager(CCBot& bot);
    std::optional<BuildOrderItem> getTopPriority() override;
};
