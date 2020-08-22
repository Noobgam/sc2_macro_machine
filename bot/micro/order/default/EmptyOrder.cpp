#include "EmptyOrder.h"
#include "../../../util/LogInfo.h"

EmptyOrder::EmptyOrder(CCBot &bot, Squad* squad) : Order(bot, squad) {
}

void EmptyOrder::onStart() {
    onEnd();
}

void EmptyOrder::onStep() { }
