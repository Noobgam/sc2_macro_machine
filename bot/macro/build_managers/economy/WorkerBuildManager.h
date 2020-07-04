#pragma once

#include "../BuildManager.h"

class CCBot;

class WorkerBuildManager : public BuildManager {
public:
	explicit WorkerBuildManager(CCBot & bot);
	std::optional<BuildOrderItem> getTopPriority() override;
};

