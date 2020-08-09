#include "EmptyOrder.h"
#include "../../../util/LogInfo.h"

EmptyOrder::EmptyOrder(CCBot &bot, Squad* squad) : Order(bot, squad) { }

void EmptyOrder::onStep() {
    LOG_DEBUG << "Executing empty order" << std::endl;
}
