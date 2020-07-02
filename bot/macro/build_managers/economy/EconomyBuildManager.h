#pragma once

#include "WorkerBuildManager.h"
#include "GasBuildManager.h"

class CCBot;

class EconomyBuildManager : public BuildManager {
private:
    WorkerBuildManager m_worker_manager;
    GasBuildManager    m_gas_manager;
public:
    explicit EconomyBuildManager(CCBot& bot);
    std::optional<BuildOrderItem> getTopPriority() override;
};
