#pragma once

#include "../Order.h"

class EmptyOrder : public Order {
public:
    explicit EmptyOrder(CCBot & bot);

    void onStep(Squad* squad) override;
};
