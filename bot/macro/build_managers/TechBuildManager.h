#pragma once
#include "BuildManager.h"

class CCBot;

class TechBuildManager : public BuildManager {
public:
    explicit TechBuildManager(CCBot& bot);
    std::optional<BuildOrderItem> getTopPriority() override;
};
