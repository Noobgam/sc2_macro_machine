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

int EconomyManager::getAvailableMineralWorkers() const {
    double minerals = 0;
    for (const auto& base : m_bot.getManagers().getBasesManager().getBases()) {
        minerals += base->getBaseWorkers()->getIdealMineralWorkers();
    }
    return minerals;
}

int EconomyManager::getAvailableVespeneWorkers() const {
    int geysers = 0;
    for (const auto& base : m_bot.getManagers().getBasesManager().getBases()) {
        geysers += base->getBaseWorkers()->getIdealMineralWorkers();
    }
    return geysers;
}
