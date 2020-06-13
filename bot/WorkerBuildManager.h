#pragma once
#include "BuildManager.h"
#include "CCBot.h"

class WorkerBuildManager : public BuildManager
{
	CCBot& m_bot;
public:
	WorkerBuildManager(CCBot & bot);
	virtual std::optional<BuildOrderItem> getTopPriority();
};

