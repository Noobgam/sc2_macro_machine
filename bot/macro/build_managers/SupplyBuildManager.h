#pragma once
#include "BuildManager.h"

class CCBot;

class SupplyBuildManager : public BuildManager {
	int getExpectedExtraSupply() const;
	int getExpectedConsumedSupply() const;
public:
	explicit SupplyBuildManager(CCBot& bot);
	std::optional<BuildOrderItem> getTopPriority() override;
};

