#include "EmptyOrder.h"

EmptyOrder::EmptyOrder(CCBot &bot) : Order(bot) { }

void EmptyOrder::onStart(Squad* squad) { }

void EmptyOrder::onStep(Squad* squad) {
    std::cerr << "Executing empty order" << std::endl;
}
