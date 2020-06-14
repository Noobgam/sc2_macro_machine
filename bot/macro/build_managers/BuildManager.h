#pragma once
#include "../BuildOrderItem.h"
#include <optional>

struct BuildManager
{
	virtual std::optional<BuildOrderItem> getTopPriority() = 0;
};

