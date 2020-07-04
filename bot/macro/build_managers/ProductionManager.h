#pragma once
#include "BuildManager.h"

class CCBot;

class ProductionManager : public BuildManager {
private:
	std::pair<float, float> approximateIncome();
public:
	explicit ProductionManager(CCBot& bot);
	std::optional<BuildOrderItem> getTopPriority() override;
};
