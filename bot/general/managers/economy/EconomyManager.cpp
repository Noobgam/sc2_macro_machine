#include "EconomyManager.h"
#include <general/CCBot.h>


EconomyManager::EconomyManager(CCBot &bot) : m_bot(bot) { }

double EconomyManager::getMineralIncome() const {
    double income = 0;
    for (const auto& baseWorkers : m_bot.getManagers().getWorkerManager().getBaseWorkers()) {
        income += baseWorkers->getMineralIncome();
    }
    return income;
}

double EconomyManager::getVespeneIncome() const {
    double income = 0;
    for (const auto& baseWorkers : m_bot.getManagers().getWorkerManager().getBaseWorkers()) {
        income += baseWorkers->getVespeneIncome();
    }
    return income;
}
