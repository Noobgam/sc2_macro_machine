#include "../general/CCBot.h"
#include "../util/Util.h"
#include "RangedManager.h"

RangedManager::RangedManager(CCBot & bot)
    : MicroManager(bot)
{

}

void RangedManager::executeMicro(const std::vector<Unit> & targets)
{
    assignTargets(targets);
}

void RangedManager::assignTargets(const std::vector<Unit> & targets)
{
    // noop
}

// get the attack priority of a type in relation to a zergling
int RangedManager::getAttackPriority(const Unit & attacker, const Unit & target)
{
    BOT_ASSERT(target.isValid(), "null unit in getAttackPriority");

    if (target.getType().isCombatUnit())
    {
        return 10;
    }

    if (target.getType().isWorker())
    {
        return 9;
    }

    return 1;
}

