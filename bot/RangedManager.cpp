#include "RangedManager.h"
#include "Util.h"
#include "CCBot.h"

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

// get a target for the ranged unit to attack
// TODO: this is the melee targeting code, replace it with something better for ranged units
Unit RangedManager::getTarget(const Unit & rangedUnit, const std::vector<Unit> & targets)
{
    BOT_ASSERT(rangedUnit.isValid(), "null melee unit in getTarget");

    int highPriority = 0;
    double closestDist = std::numeric_limits<double>::max();
    Unit closestTarget;

    // for each target possiblity
    for (auto & targetUnit : targets)
    {
        BOT_ASSERT(targetUnit.isValid(), "null target unit in getTarget");

        int priority = getAttackPriority(rangedUnit, targetUnit);
        float distance = Util::Dist(rangedUnit, targetUnit);

        // if it's a higher priority, or it's closer, set it
        if (!closestTarget.isValid() || (priority > highPriority) || (priority == highPriority && distance < closestDist))
        {
            closestDist = distance;
            highPriority = priority;
            closestTarget = targetUnit;
        }
    }

    return closestTarget;
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

