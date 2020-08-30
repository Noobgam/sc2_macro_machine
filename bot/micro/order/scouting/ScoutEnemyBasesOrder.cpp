#include <util/LogInfo.h>
#include <util/Util.h>
#include "ScoutEnemyBasesOrder.h"
#include "general/CCBot.h"

ScoutEnemyBasesOrder::ScoutEnemyBasesOrder(CCBot &bot, Squad *squad, const std::vector<const BaseLocation*>& baseLocations) :
    Order(bot, squad),
    m_baseLocationsToScout(baseLocations)
{
    BOT_ASSERT(!m_squad->isEmpty(), "Cannot scout with empty squad");
    m_scout = { *m_squad->units().begin() };
    m_nextBaseIt = m_baseLocationsToScout.begin();
}

void ScoutEnemyBasesOrder::onStart() {
    if (m_nextBaseIt == m_baseLocationsToScout.end()) {
        onEnd();
    }
}

void ScoutEnemyBasesOrder::onStep() {
    if (m_scout.has_value()) {
        const auto& scout = m_scout.value();
        if (Util::Dist(scout, (*m_nextBaseIt)->getPosition()) <= 1) {
            scoutAround(); // we are on base
            if (++m_nextBaseIt == m_baseLocationsToScout.end()) {
                onEnd();
                return;
            }
        }
        m_scout.value()->move((*m_nextBaseIt)->getPosition());
    }
}

void ScoutEnemyBasesOrder::onUnitAdded(const Unit *unit) {
    BOT_ASSERT(false, "Unit addition is not supported");
}

void ScoutEnemyBasesOrder::onUnitRemoved(const Unit *unit) {
    if (m_scout == unit) {
        m_scout = {};
    }
}

void ScoutEnemyBasesOrder::scoutAround() {
    LOG_DEBUG << "Scouting base " << (*m_nextBaseIt)->getBaseId() << BOT_ENDL;
}
