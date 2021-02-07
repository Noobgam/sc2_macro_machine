#pragma once

#include <micro/order/Order.h>
#include <map>

// proof of concept-ish
class SimplePrismJuggleOrder : public Order {
    const BaseLocation* targetBase;
public:

    std::map<CCUnitID, int> weaponCdFrame;
    SimplePrismJuggleOrder(CCBot &bot, Squad *squad, const BaseLocation* targetBase);

public:
    void onStep() override;

    void onEnd() override;



};


