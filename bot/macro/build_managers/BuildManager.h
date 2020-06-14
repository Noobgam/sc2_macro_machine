#pragma once
#include "../../BuildOrderQueue.h"
#include <optional>

struct BuildManager
{
	virtual std::optional<BuildOrderItem> getTopPriority() = 0;
};

