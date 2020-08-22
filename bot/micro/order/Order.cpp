#include "Order.h"

Order::Order(CCBot &bot, Squad* squad) : m_bot(bot), m_squad(squad) { }

void Order::onStart() { }

bool Order::isCompleted() const {
    return m_completed;
}

void Order::onEnd() {
    m_completed = true;
    if (m_nextOrder.has_value()) {
        m_squad->setOrder(m_nextOrder.value());
    }
}

void Order::draw() const {
}

void Order::onUnitAdded(const Unit *unit) { }

void Order::onUnitRemoved(const Unit *unit) { }

void Order::chain(const std::shared_ptr<Order>& order) {
    Order* lastOrder = this;
    while (lastOrder->m_nextOrder.has_value()) {
        lastOrder = m_nextOrder.value().get();
    }
    lastOrder->m_nextOrder = order;
}