#include "EmptyOrder.h"
#include "../../../util/LogInfo.h"

EmptyOrder::EmptyOrder(CCBot &bot) : Order(bot) { }

void EmptyOrder::onStep(Squad* squad) {
    LOG_DEBUG << "Executing empty order" << std::endl;
}
