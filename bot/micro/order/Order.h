#pragma once

#include "../squads/Squad.h"

class Order {
protected:
    CCBot & m_bot;
    Squad* m_squad;
    bool m_completed = false;

    virtual void onEnd();
public:
    explicit Order(CCBot & bot, Squad* squad);

    virtual void onStart();
    virtual void onStep() = 0;

    bool isCompleted() const;
};