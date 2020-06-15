#pragma once

#include "BuildManager.h"
#include "../../general/CCBot.h"

class TechBuildManager : public BuildManager
{
    CCBot & m_bot;

public:
    TechBuildManager(CCBot& bot);
    virtual std::optional<BuildOrderItem> getTopPriority();
};
