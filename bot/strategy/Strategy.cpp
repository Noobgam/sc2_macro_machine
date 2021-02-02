#include <macro/build_managers/SupplyBuildManager.h>
#include <macro/build_managers/economy/EconomyBuildManager.h>
#include <macro/build_managers/ForgeBuildManager.h>
#include <macro/build_managers/TechBuildManager.h>
#include <macro/build_managers/UnitHireManager.h>
#include <macro/build_managers/ProductionManager.h>
#include "Strategy.h"
#include "general/CCBot.h"

Strategy::Strategy(CCBot& bot, HighLevelStrategy strategy)
    : m_bot(bot)
    , currentStrategy(NONE)
    , targetStrategy(strategy)
{ }

void Strategy::onFrame() {
    if (currentStrategy != targetStrategy) {
        m_bot.Commander().getMacroManager().getMutableManagers().clear();
        auto& managers = m_bot.Commander().getMacroManager().getMutableManagers();
        switch (targetStrategy) {
            // TODO: proper destruction?

            case HighLevelStrategy::MACRO:
                managers.emplace_back(std::make_unique<SupplyBuildManager>(m_bot));
                managers.emplace_back(std::make_unique<EconomyBuildManager>(m_bot));
                managers.emplace_back(std::make_unique<ProductionManager>(m_bot));
                managers.emplace_back(std::make_unique<UnitHireManager>(m_bot));
                managers.emplace_back(std::make_unique<TechBuildManager>(m_bot));

                setGasGoal({});
                setWorkersGoal({});
                setExpandGoal({});
                break;
            case HighLevelStrategy::CANNONS:
                managers.emplace_back(std::make_unique<SupplyBuildManager>(m_bot));
                managers.emplace_back(std::make_unique<EconomyBuildManager>(m_bot));
                managers.emplace_back(std::make_unique<ForgeBuildManager>(m_bot));

                setGasGoal({0});
                setWorkersGoal({16});
                setExpandGoal({1});
                break;
            default:
                break;
        }
        currentStrategy = targetStrategy;
    }
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

void Strategy::setTargetStrategy(Strategy::HighLevelStrategy strategy) {
    targetStrategy = strategy;
}

Strategy::HighLevelStrategy Strategy::getCurrentStrategy() {
    return currentStrategy;
}
