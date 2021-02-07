#pragma once

#include "../squads/Squad.h"

class Order {
protected:
    CCBot & m_bot;
    Squad* m_squad;
    bool m_completed = false;
    std::optional<std::shared_ptr<Order>> m_nextOrder = {};

public:
    explicit Order(CCBot & bot, Squad* squad);

    virtual void onStart();
    virtual void onStep() = 0;
    virtual void onEnd();
    virtual void draw() const;

    virtual void onUnitAdded(const Unit* unit);
    virtual void onUnitRemoved(const Unit* unit);

    bool isCompleted() const;
    void chain(const std::shared_ptr<Order>& order);
};