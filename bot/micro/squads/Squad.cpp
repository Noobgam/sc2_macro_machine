#include "Squad.h"
#include "../order/Orders.h"

Squad::Squad(CCBot & bot, SquadID id):
        m_bot(bot),
        m_id(id),
        m_order(std::make_shared<EmptyOrder>(bot, this)) { }

void Squad::addUnits(const std::set<const Unit*> & units) {
    for (auto unit : units) {
        m_units.insert(unit);
        m_order->onUnitAdded(unit);
    }
}

void Squad::removeUnits(const std::set<const Unit*> & units) {
    for (auto unit : units) {
        m_units.erase(unit);
        m_order->onUnitRemoved(unit);
    }
}

void Squad::clear() {
    for (auto & unit : m_units) {
        m_order->onUnitRemoved(unit);
    }
    m_units.clear();
}

const std::set<const Unit*> & Squad::units() const {
    return m_units;
}

bool Squad::isEmpty() const {
    return m_units.empty();
}

SquadID Squad::getId() const {
    return m_id;
}

const std::shared_ptr<Order> &Squad::getOrder() const {
    return m_order;
}

void Squad::setOrder(const std::shared_ptr<Order>& order) {
    m_order = order;
    m_order->onStart();
}

void Squad::setOrder(std::shared_ptr<Order>&& order) {
    m_order = order;
    m_order->onStart();
}

void Squad::act() {
    if (!m_order->isCompleted()) {
        m_order->onStep();
    }
}
