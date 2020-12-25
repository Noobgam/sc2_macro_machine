#pragma once
#include "BuildManager.h"

class CCBot;

class ForgeBuildManager : public BuildManager {
public:
    explicit ForgeBuildManager(CCBot& bot);
    std::optional<BuildOrderItem> getTopPriority() override;
};


