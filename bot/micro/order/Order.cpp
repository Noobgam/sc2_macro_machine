#include "Order.h"

Order::Order(CCBot &bot, Squad* squad) : m_bot(bot), m_squad(squad) { }

void Order::onStart() { }

bool Order::isCompleted() const {
    return m_completed;
}

void Order::onEnd() {
    m_completed = true;
}
