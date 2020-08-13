#include "Strategy.h"
#include "general/CCBot.h"

Strategy::Strategy(CCBot& bot) : m_bot(bot) { }

void Strategy::onFrame() {
    int seconds = m_bot.GetCurrentFrame() / 22.4;
    if (seconds % 60 == 20) {
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
    }
}