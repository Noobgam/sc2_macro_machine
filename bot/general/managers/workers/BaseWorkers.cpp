#include "BaseWorkers.h"

#include <general/CCBot.h>
#include <micro/order/workers/CollectMineralsOrder.h>

BaseWorkers::BaseWorkers(CCBot &bot, const BaseLocation * base): m_bot(bot), m_baseLocation(base) {
    m_mineralSquad = m_bot.getManagers().getSquadManager().createNewSquad();
    m_vespeneSquad = m_bot.getManagers().getSquadManager().createNewSquad();

    m_mineralSquad->setOrder(std::make_shared<CollectMineralsOrder>(m_bot, m_mineralSquad, base));
}

const BaseLocation *BaseWorkers::getBaseLocation() const {
    return m_baseLocation;
}

void BaseWorkers::deform() {
    m_bot.getManagers().getSquadManager().deformSquad(m_mineralSquad);
    m_bot.getManagers().getSquadManager().deformSquad(m_vespeneSquad);
}

const Squad *BaseWorkers::getMineralSquad() const {
    return m_mineralSquad;
}

const Squad *BaseWorkers::getVespeneSquad() const {
    return m_vespeneSquad;
}

int BaseWorkers::getActiveMineralWorkersNumber() const {
    return m_mineralSquad->units().size();
}

int BaseWorkers::getIdealMineralWorkersNumber() const {
    return m_baseLocation->getMinerals().size() * 2;
}

int BaseWorkers::getMaximumMineralWorkersNumber() const {
    return m_baseLocation->getMinerals().size() * 3;
}

int BaseWorkers::getActiveVespeneWorkers() const {
    return m_vespeneSquad->units().size();
}

int BaseWorkers::getIdealVespeneWorkers() const {
    return 0;
}

int BaseWorkers::getMaximumVespeneWorkers() const {
    return 0;
}

float BaseWorkers::getMineralIncome() const {
    return 0;
}

float BaseWorkers::getVespeneIncome() const {
    return 0;
}

void BaseWorkers::assignToMineral(const Unit *unit) {
    m_bot.getManagers().getSquadManager().transferUnits({unit}, m_mineralSquad);
}

void BaseWorkers::assignToVespene(const Unit *unit) {
    m_bot.getManagers().getSquadManager().transferUnits({unit}, m_vespeneSquad);
}
