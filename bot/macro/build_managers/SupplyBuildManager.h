#pragma once
#include "BuildManager.h"
#include "../../general/CCBot.h"

class SupplyBuildManager : public BuildManager
{
	CCBot & m_bot;

	int getExpectedExtraSupply();

public:
	SupplyBuildManager(CCBot& bot);
	virtual std::optional<BuildOrderItem> getTopPriority();
};

