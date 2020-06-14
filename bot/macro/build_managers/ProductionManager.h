#pragma once
#include "BuildManager.h"
#include "../../general/CCBot.h"

class ProductionManager : public BuildManager
{
private:
	CCBot& m_bot;
	// returns pair <minerals, gas>
	std::pair<float, float> approximateIncome();
public:
	ProductionManager(CCBot& bot);
	virtual std::optional<BuildOrderItem> getTopPriority();
};
