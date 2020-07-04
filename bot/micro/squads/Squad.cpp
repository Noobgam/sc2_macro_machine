#include "Squad.h"
#include "../order/Orders.h"

Squad::Squad(CCBot & bot, SquadID id):
        m_bot(bot),
        m_id(id),
        m_order(std::make_shared<EmptyOrder>(bot)) { }

void Squad::addUnits(const std::set<const Unit*> & units) {
    for (auto unit : units) {
        m_units.insert(unit);
    }
}

void Squad::removeUnits(const std::set<const Unit*> & units) {
    for (auto unit : units) {
        m_units.erase(unit);
    }
}

void Squad::clear() {
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

void Squad::setOrder(const std::shared_ptr<Order>& order) {
    m_order = order;
    m_order->onStart(this);
}

void Squad::act() {
    m_order->onStep(this);
}
