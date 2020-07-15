#include "BuildingOrder.h"
#include "../../../util/Util.h"
#include "../../../general/CCBot.h"

BuildingOrder::BuildingOrder(CCBot &bot, Squad *squad, const BuildingTask *task): Order(bot, squad), m_task(task) { }

void BuildingOrder::onStart() {
    auto& worker = *m_squad->units().begin();
    CCPosition position = m_task->getPosition();
    if (m_task->getType().isRefinery()) {
        // first we find the geyser at the desired location
        auto& units = m_bot.UnitInfo().getUnits(Players::Neutral);
        auto geyser = std::find_if(units.begin(), units.end(), [&position](const Unit* const& unit) {
            return unit->getType().isGeyser() && Util::Dist(Util::GetPosition(position), unit->getPosition()) < 1;
        });
        if (geyser != units.end()) {
            worker->buildTarget(m_task->getType(), **geyser);
        }
    } else {
        worker->build(m_task->getType(), position);
    }
}

void BuildingOrder::onStep() {
    if (m_task->getStatus() == BuildingStatus::FAILED || m_task->getStatus() == BuildingStatus::IN_PROGRESS) {
        onEnd();
    }
}
