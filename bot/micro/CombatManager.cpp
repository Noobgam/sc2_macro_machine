#include "CombatManager.h"

#include "general/CCBot.h"
#include "order/Orders.h"

#include <util/LogInfo.h>
#include <util/Util.h>

CombatManager::CombatManager(CCBot & bot) :
    m_bot(bot),
    m_boostModule(bot),
    m_scoutModule(bot)
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
            const auto& base = getAttackTarget();
            if (base.has_value()) {
                mainSquad->setOrder(std::make_shared<AttackWithKiting>(
                    m_bot,
                    mainSquad,
                    base.value()->getPosition()
                ));
            }
        }
    } else if (mainSquad->units().size() > 2) {
        if (dynamic_cast<AttackWithKiting*>(mainSquad->getOrder().get()) == nullptr) {
            orderToGroup(mainSquad);
        }
    }
    if (mainSquad->getOrder()->isCompleted() &&
        dynamic_cast<AttackWithKiting*>(mainSquad->getOrder().get()) != nullptr
    ) {
        const auto& base = getAttackTarget();
        if (base.has_value()) {
            mainSquad->setOrder(std::make_shared<AttackWithKiting>(m_bot, mainSquad, base.value()->getPosition()));
        }
    }
    m_scoutModule.onFrame();
    m_boostModule.onFrame();
}

void CombatManager::orderToGroup(Squad* squad) {
    int startId = m_bot.getManagers().getBasesManager().getStartLocation()->getBaseId();
    auto& orderedBases = m_bot.Map().getStaticMapMeta().getOrderedBasesByStartLocationId().at(startId);
    int targetBaseId = -1;
    for (int i = 0; i < 2; ++i) {
        int baseId = orderedBases[i];
        if (m_bot.getManagers().getBasesManager().isBaseOccupied(baseId)) {
            targetBaseId = baseId;
        }
    }
    auto enemyBaseLocation = getAttackTarget();
    if (targetBaseId == -1 || !enemyBaseLocation.has_value()) {
        LOG_DEBUG << "No bases, no idea where to group" << BOT_ENDL;
    } else {
        auto base = m_bot.Bases().getBaseLocation(targetBaseId);
        auto path = base->getDistanceMap().getPathTo(
            enemyBaseLocation.value()->getDepotActualPosition()
        );
        squad->setOrder(std::make_shared<GroupAroundOrder>(
            m_bot,
            squad,
            Util::GetTileCenter(path[10]),
            true
        ));
    }
}

const std::optional<const BaseLocation*> CombatManager::getAttackTarget() {
    const auto & bases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getOccupiedEnemyBaseLocations();
    if (!bases.empty()) {
        return *bases.begin();
    } else {
        const auto & allBases = m_bot.getManagers().getEnemyManager().getEnemyBasesManager().getAllExpectedEnemyBaseLocations();
        if (!allBases.empty()) {
            return *allBases.begin();
        }
        return {};
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

