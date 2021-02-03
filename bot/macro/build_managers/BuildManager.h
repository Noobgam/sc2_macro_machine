#pragma once
#include "../BuildOrderItem.h"
#include <optional>

class BuildManager {
protected:
    CCBot & m_bot;
public:
    explicit BuildManager(CCBot & bot);

	virtual std::optional<BuildOrderItem> getTopPriority() = 0;
    virtual ~BuildManager();
};

