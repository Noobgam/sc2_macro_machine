#pragma once

#include "../../../general/CCBot.h"

class GasBuildManager : public BuildManager
{
private:
    CCBot& m_bot;
public:
    GasBuildManager(CCBot& bot);
    virtual std::optional<BuildOrderItem> getTopPriority();
};
