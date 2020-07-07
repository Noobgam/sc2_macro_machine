#include "BuildingOrder.h"

BuildingOrder::BuildingOrder(CCBot &bot, Squad *squad, const BuildingTask *task): Order(bot, squad), m_task(task) { }

void BuildingOrder::onStart() {
    auto& worker = *m_squad->units().begin();
    worker->build(m_task->getType(), m_task->getPosition());
}

void BuildingOrder::onStep() {
    if (m_task->getStatus() == BuildingStatus::FAILED || m_task->getStatus() == BuildingStatus::IN_PROGRESS) {
        onEnd();
    }
}
