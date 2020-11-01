#pragma once
#include "BuildManager.h"

class CCBot;

class AggressiveTechBuildManager : public BuildManager {
public:
    explicit AggressiveTechBuildManager(CCBot& bot);
    std::optional<BuildOrderItem> getTopPriority() override;
};
