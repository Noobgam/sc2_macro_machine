#pragma once

#include "../BuildManager.h"

class CCBot;

class ExpandBuildManager: BuildManager {
public:
    explicit ExpandBuildManager(CCBot & bot);
    std::optional<BuildOrderItem> getTopPriority() override;
};
