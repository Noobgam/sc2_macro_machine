#pragma once

#include "../../general/model/Unit.h"

typedef unsigned long long SquadID;

class SquadManager;
class Order;

class Squad {
private:
    CCBot & m_bot;

    friend SquadManager;
    std::set<const Unit*> m_units;
    const SquadID m_id;

    std::shared_ptr<Order> m_order;

    void addUnit(const Unit* unit);
    void addUnits(const std::set<const Unit*> & units);
    void removeUnits(const std::set<const Unit*> & units);
    void clear();
public:
    Squad(CCBot & bot, SquadID id);

    SquadID getId() const;

    bool isEmpty() const;
    const std::set<const Unit*> & units() const;
    const std::shared_ptr<Order>& getOrder() const;

    void setOrder(const std::shared_ptr<Order>& order);
    void setOrder(std::shared_ptr<Order>&& order);
    void act();
};