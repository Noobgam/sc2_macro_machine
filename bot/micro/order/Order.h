#pragma once

#include "../squads/Squad.h"

class Order {
protected:
    CCBot & m_bot;

public:
    explicit Order(CCBot & bot);

    virtual void onStart(Squad* squad);
    virtual void onStep(Squad* squad) = 0;
};