#include <util/LogInfo.h>
#include "BuildingOrder.h"
#include "../../../util/Util.h"
#include "../../../general/CCBot.h"

BuildingOrder::BuildingOrder(CCBot &bot, Squad *squad, BuildingTask *task): Order(bot, squad), m_task(task) { }

void BuildingOrder::onStart() {
}

void BuildingOrder::onStep() {
    if (m_task->getStatus() == BuildingStatus::NEW) {
        auto &worker = *m_squad->units().begin();
        worker->move(m_task->getPosition());
        LOG_DEBUG << worker->getUnitPtr()->tag << " is moving to building position." << BOT_ENDL;
        LOG_DEBUG << "Will take " << m_bot.Map().getWalkTime(worker, m_task->getPosition()) << " to move." << BOT_ENDL;
        m_task->scheduled();
    }
    if (m_task->getStatus() == BuildingStatus::SCHEDULED) {
        auto &worker = *m_squad->units().begin();
        auto position = m_task->getPosition();
        const auto& type = m_task->getType();
        if (m_bot.Data(type).mineralCost <= m_bot.GetMinerals() && m_bot.Data(type).gasCost <= m_bot.GetGas()) {
            if (m_task->getType().isRefinery()) {
                // first we find the geyser at the desired location
                auto &units = m_bot.UnitInfo().getUnits(Players::Neutral);
                auto geyser = std::find_if(units.begin(), units.end(), [&position](const Unit *const &unit) {
                    return unit->getType().isGeyser() && Util::Dist(Util::GetPosition(position), unit->getPosition()) < 1;
                });
                if (geyser != units.end()) {
                    worker->buildTarget(m_task->getType(), **geyser);
                    m_task->ordered();
                }
            } else {
                worker->build(m_task->getType(), position);
                m_task->ordered();
            }
        }
    }
    if (m_task->getStatus() == BuildingStatus::FAILED || m_task->getStatus() == BuildingStatus::IN_PROGRESS) {
        onEnd();
    }
}
