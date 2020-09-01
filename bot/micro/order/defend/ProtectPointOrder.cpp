#include "ProtectPointOrder.h"

ProtectPointOrder::ProtectPointOrder(CCBot &bot, Squad *squad, CCPosition point)
    : Order(bot, squad),
      m_point(point)
{ }

void ProtectPointOrder::onStep() {

}
