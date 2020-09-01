#include "EconomyManager.h"
#include <general/CCBot.h>


EconomyManager::EconomyManager(CCBot &bot) : m_bot(bot) { }

void EconomyManager::onFrame() {
    m_minerals = m_bot.Observation()->GetMinerals();
    m_vespene = m_bot.Observation()->GetVespene();
}

int EconomyManager::getResource(ResourceType type) const {
    switch (type) {
        case ResourceType::MINERAL:
            return m_minerals;
        case ResourceType::VESPENE:
            return m_vespene;
    }
    BOT_ASSERT(false, "Unknown resource type");
    return 0;
}

void EconomyManager::useResource(ResourceType type, int amount) {
    switch (type) {
        case ResourceType::MINERAL:
            m_minerals -= amount;
        case ResourceType::VESPENE:
            m_vespene -= amount;
    }
}

double EconomyManager::getMineralIncome() const {
    double income = 0;
    for (const auto& base : m_bot.getManagers().getBasesManager().getBases()) {
        income += base->getBaseWorkers()->getMineralIncome();
    }
    return income;
}

double EconomyManager::getVespeneIncome() const {
    double income = 0;
    for (const auto& base : m_bot.getManagers().getBasesManager().getBases()) {
        income += base->getBaseWorkers()->getVespeneIncome();
    }
    return income;
}

int EconomyManager::getMineralWorkersPositions() const {
    double mineralWorkersPositions = 0;
    for (const auto& base : m_bot.getManagers().getBasesManager().getBases()) {
        mineralWorkersPositions += base->getBaseWorkers()->getIdealMineralWorkers();
    }
    return mineralWorkersPositions;
}

int EconomyManager::getVespeneWorkersPositions() const {
    int geyserWorkersPositions = 0;
    for (const auto& base : m_bot.getManagers().getBasesManager().getBases()) {
        for (const auto & geyser : base->getBaseLocation()->getGeysers()) {
            if (geyser->getResourceAmount() > 0) {
                geyserWorkersPositions += 3;
            }
        }
    }
    return geyserWorkersPositions;
}

void EconomyManager::reserveResource(ResourceType type, int amount) {
    switch (type) {
        case ResourceType::MINERAL:
            m_reservedMinerals += amount;
        case ResourceType::VESPENE:
            m_reservedVespene += amount;
    }
}

void EconomyManager::freeResource(ResourceType type, int amount) {
    switch (type) {
        case ResourceType::MINERAL:
            m_reservedMinerals -= amount;
        case ResourceType::VESPENE:
            m_reservedVespene -= amount;
    }
}

float EconomyManager::getAvailableResources(ResourceType type, float seconds) const {
    switch (type) {
        case ResourceType::MINERAL:
            return m_minerals - m_reservedMinerals + seconds * getMineralIncome();
        case ResourceType::VESPENE:
            return m_vespene - m_reservedVespene + seconds * getVespeneIncome();
    }
    BOT_ASSERT(false, "Unknown resource type");
    return 0;
}
