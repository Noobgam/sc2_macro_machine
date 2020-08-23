#include "EconomyManager.h"
#include <general/CCBot.h>


EconomyManager::EconomyManager(CCBot &bot) : m_bot(bot) { }

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
