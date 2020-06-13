#pragma once
#include "BuildManager.h"
#include "CCBot.h"

class UnitHireManager : public BuildManager
{
	CCBot& m_bot;
public:
	UnitHireManager(CCBot& bot);
	virtual std::optional<BuildOrderItem> getTopPriority();
};