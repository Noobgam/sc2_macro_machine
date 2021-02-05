#pragma once

#include <micro/order/Order.h>
#include <map>

// proof of concept-ish
class SimplePrismJuggleOrder : public Order {
public:
    std::map<CCUnitID, int> weaponCdFrame;
    SimplePrismJuggleOrder(CCBot &bot, Squad *squad);

public:
    void onStep() override;

    void onEnd() override;



};


