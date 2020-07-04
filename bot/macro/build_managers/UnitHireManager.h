#pragma once
#include "BuildManager.h"

class CCBot;

class UnitHireManager : public BuildManager {
public:
	explicit UnitHireManager(CCBot& bot);
	std::optional<BuildOrderItem> getTopPriority() override;
};