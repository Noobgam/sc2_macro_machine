#pragma once

#include "../BuildManager.h"

class CCBot;

class GasBuildManager : public BuildManager {
public:
    explicit GasBuildManager(CCBot& bot);
    std::optional<BuildOrderItem> getTopPriority() override;
};
