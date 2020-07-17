#include "BaseWorkers.h"

#include <general/CCBot.h>
#include <micro/order/workers/CollectMineralsOrder.h>
#include <micro/order/workers/CollectVespeneOrder.h>

BaseWorkers::BaseWorkers(CCBot &bot, const Base * base): m_bot(bot), m_base(base) {
    m_mineralSquad = m_bot.getManagers().getSquadManager().createNewSquad();
    m_vespeneSquad = m_bot.getManagers().getSquadManager().createNewSquad();

    m_mineralSquad->setOrder(std::make_shared<CollectMineralsOrder>(m_bot, m_mineralSquad, base->getBaseLocation()));
    m_vespeneSquad->setOrder(std::make_shared<CollectVespeneOrder>(m_bot, m_mineralSquad, base));
}

const Base *BaseWorkers::getBase() const {
    return m_base;
}

void BaseWorkers::deform() {
    m_bot.getManagers().getSquadManager().deformSquad(m_mineralSquad);
    m_bot.getManagers().getSquadManager().deformSquad(m_vespeneSquad);
}

const Squad* BaseWorkers::getResourceSquad(ResourceType type) const {
    switch (type) {
        case ResourceType::MINERAL:
            return getMineralSquad();
        case ResourceType::GEYSER:
            return getVespeneSquad();
    }
    BOT_BREAK
    return getMineralSquad();
}

const Squad *BaseWorkers::getMineralSquad() const {
    return m_mineralSquad;
}

const Squad *BaseWorkers::getVespeneSquad() const {
    return m_vespeneSquad;
}

int BaseWorkers::getActiveResourceWorkers(ResourceType type) const {
    switch (type) {
        case ResourceType::MINERAL:
            return getActiveMineralWorkers();
        case ResourceType::GEYSER:
            return getActiveVespeneWorkers();
    }
    BOT_BREAK
    return getActiveMineralWorkers();
}

int BaseWorkers::getActiveMineralWorkers() const {
    return m_mineralSquad->units().size();
}

int BaseWorkers::getActiveVespeneWorkers() const {
    return m_vespeneSquad->units().size();
}

int BaseWorkers::getIdealResourceWorkers(ResourceType type) const {
    switch (type) {
        case ResourceType::MINERAL:
            return getIdealMineralWorkers();
        case ResourceType::GEYSER:
            return getIdealVespeneWorkers();
    }
    BOT_BREAK
    return getIdealMineralWorkers();
}

int BaseWorkers::getIdealMineralWorkers() const {
    return m_base->getBaseLocation()->getMinerals().size() * 2;
}

int BaseWorkers::getIdealVespeneWorkers() const {
    return m_base->getActiveAssimilators().size() * 2;
}

int BaseWorkers::getMaximumResourceWorkers(ResourceType type) const {
    switch (type) {
        case ResourceType::MINERAL:
            return getMaximumMineralWorkers();
        case ResourceType::GEYSER:
            return getMaximumVespeneWorkers();
    }
    BOT_BREAK
    return getMaximumMineralWorkers();
}

int BaseWorkers::getMaximumMineralWorkers() const {
    return m_base->getBaseLocation()->getMinerals().size() * 3;
}

int BaseWorkers::getMaximumVespeneWorkers() const {
    return m_base->getActiveAssimilators().size() * 3;
}

double BaseWorkers::getResourceIncome(ResourceType type) const {
    switch (type) {
        case ResourceType::MINERAL:
            return getMineralIncome();
        case ResourceType::GEYSER:
            return getVespeneIncome();
    }
    BOT_BREAK
    return getMineralIncome();
}

double BaseWorkers::getMineralIncome() const {
    int workers = getActiveMineralWorkers();
    int bestWorkers = std::min(workers, getIdealMineralWorkers());
    int thirdWorkers = std::min(workers, getMaximumMineralWorkers()) - bestWorkers;
    // first and second gives 55-60 depending on distance
    // 3 workers on patch gives 143 per minute
    return (bestWorkers * 58 + thirdWorkers * 27.0) / 60;
}

double BaseWorkers::getVespeneIncome() const {
    int workers = getActiveVespeneWorkers();
    int bestWorkers = std::min(workers, getIdealVespeneWorkers());
    int thirdWorkers = std::min(workers, getMaximumVespeneWorkers()) - bestWorkers;
    // first and second gives 55-60 depending on distance
    // 3 workers on patch gives 143 per minute
    return (bestWorkers * 58 + thirdWorkers * 27.0) / 60;
}

void BaseWorkers::assignToResource(ResourceType type, const Unit *unit) {
    switch (type) {
        case ResourceType::MINERAL:
            assignToMineral(unit);
            return;
        case ResourceType::GEYSER:
            assignToVespene(unit);
            return;
    }
    BOT_BREAK
}

void BaseWorkers::assignToMineral(const Unit *unit) {
    m_bot.getManagers().getSquadManager().transferUnits({unit}, m_mineralSquad);
}

void BaseWorkers::assignToVespene(const Unit *unit) {
    m_bot.getManagers().getSquadManager().transferUnits({unit}, m_vespeneSquad);
}
