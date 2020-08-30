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
        CCPosition position = m_task->getPosition();
        if (m_task->getType().isRefinery()) {
            // first we find the geyser at the desired location
            auto &units = m_bot.UnitInfo().getUnits(Players::Neutral);
            auto geyser = std::find_if(units.begin(), units.end(), [&position](const Unit *const &unit) {
              return unit->getType().isGeyser() && Util::Dist(Util::GetPosition(position), unit->getPosition()) < 1;
            });
            if (geyser != units.end()) {
                worker->buildTarget(m_task->getType(), **geyser);
            }
        } else {
            worker->build(m_task->getType(), position);
            LOG_DEBUG << worker->getUnitPtr()->tag << " is building" << BOT_ENDL;
            LOG_DEBUG << "Will take " << m_bot.Map().getWalkTime(*worker, position) << " to move to position"
                      << BOT_ENDL;
        }
        m_task->scheduled();
    }

    if (m_task->getStatus() == BuildingStatus::FAILED || m_task->getStatus() == BuildingStatus::IN_PROGRESS) {
        onEnd();
    }
}
