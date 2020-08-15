#include "CombatManager.h"

#include "general/CCBot.h"
#include "order/Orders.h"

#include <util/LogInfo.h>
#include <util/Util.h>

CombatManager::CombatManager(CCBot & bot) :
    m_bot(bot),
    m_boostModule(bot)
{}

void CombatManager::onStart() {
    mainSquad = m_bot.getManagers().getSquadManager().createNewSquad();
    leftOverSquad = m_bot.getManagers().getSquadManager().createNewSquad();
}

void CombatManager::onFrame() {
    reformSquads();
    if (mainSquad->units().size() >= 10) {
        Order* order = mainSquad->getOrder().get();
        if (order->isCompleted() || dynamic_cast<AttackWithKiting*>(order) == nullptr) {
            auto & base = *m_bot.Bases().getOccupiedBaseLocations(Players::Enemy).begin();
            mainSquad->setOrder(std::make_shared<AttackWithKiting>(m_bot, mainSquad, base->getPosition()));
        }
    } else if (mainSquad->units().size() > 2) {
        orderToGroup(mainSquad);
    }
    m_boostModule.onFrame();
}

void CombatManager::orderToGroup(Squad* squad) {
    int startId = m_bot.Bases().getPlayerStartLocation(Players::Self)->getBaseId();
    auto& orderedBases = m_bot.Map().getStaticMapMeta().getOrderedBasesByStartLocationId().at(startId);
    int targetBaseId = -1;
    for (int i = 0; i < 2; ++i) {
        int baseId = orderedBases[i];
        if (m_bot.getManagers().getBasesManager().isBaseOccupied(baseId)) {
            targetBaseId = baseId;
        }
    }
    if (targetBaseId == -1) {
        LOG_DEBUG << "No bases, no idea where to group" << BOT_ENDL;
    } else {
        auto base = m_bot.Bases().getBaseLocation(targetBaseId);
        auto enemyStartLocation = m_bot.Bases().getPlayerStartLocation(Players::Enemy);
        auto path = base->getDistanceMap().getPathTo(
            enemyStartLocation->getDepotActualPosition()
        );
        squad->setOrder(std::make_shared<GroupAroundOrder>(
            m_bot,
            squad,
            Util::GetTileCenter(path[10]),
            true
        ));
    }
}

void CombatManager::reformSquads() {
    auto& squadManager = m_bot.getManagers().getSquadManager();
    std::set<const Unit*> toTransfer;
    for (auto unit : squadManager.getUnassignedSquad()->units()) {
        if (unit->getType().isRegularUnit() && !unit->getType().isWorker()) {
            toTransfer.insert(unit);
        }
    }
    if (dynamic_cast<AttackWithKiting*>(mainSquad->getOrder().get()) == nullptr) {
        squadManager.transferUnits(toTransfer, mainSquad);
    } else {
        squadManager.transferUnits(toTransfer, leftOverSquad);
        if (leftOverSquad->units().size() >= 8) {
            squadManager.transferUnits(leftOverSquad, mainSquad);
        } else {
            orderToGroup(leftOverSquad);
        }
    }
}

