#include "Strategy.h"
#include "general/CCBot.h"

Strategy::Strategy(CCBot& bot) : m_bot(bot) { }

void Strategy::onFrame() {
    int seconds = m_bot.GetCurrentFrame() / 22.4;
    if ((m_lastUpdate == 0 && seconds >= 60) || (seconds - m_lastUpdate >= 120)) {
        const auto& bases = m_bot.Bases().getBaseLocations();
        const auto& ourBases = m_bot.getManagers().getBasesManager().getBases();
        const auto& enemyBases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getAllExpectedEnemyBaseLocations();
        for (const auto& base : bases) {
            const auto& ourIt = std::find_if(ourBases.begin(), ourBases.end(), [base](auto b) {
                return b->getBaseLocation() == base;
            });
            const auto enemyIt = std::find(enemyBases.begin(), enemyBases.end(), base);
            if (ourIt == ourBases.end() && enemyIt == enemyBases.end()) {
                m_bot.getManagers().getEnemyManager().getEnemyBasesManager().expectAsOccupied(base);
            }
        }
        m_lastUpdate = seconds;
    }
}

std::optional<int> Strategy::getGasGoal() const {
    return m_gasGoal;
}

void Strategy::setGasGoal(std::optional<int> goalO) {
    m_gasGoal = goalO;
}

std::optional<int> Strategy::getWorkersGoal() const {
    return m_workersGoal;
}

void Strategy::setWorkersGoal(std::optional<int> goalO) {
    m_workersGoal = goalO;
}

std::optional<int> Strategy::getExpandGoal() const {
    return m_expandGoal;
}

void Strategy::setExpandGoal(std::optional<int> goalO) {
    m_expandGoal = goalO;
}
