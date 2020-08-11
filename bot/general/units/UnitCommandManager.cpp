#include "UnitCommandManager.h"

#include <general/CCBot.h>

// if the same command is issued again more often than threshold, it will be skipped
constexpr int FRAME_SKIP_THRESHOLD = 5;

namespace NPrivate {
    bool comparePositions(const CCPosition& lhs, const CCPosition& rhs) {
        if (lhs.x != rhs.x) return lhs.x < rhs.x;
        return lhs.y < rhs.y;
    }

    bool CommandAttribute::operator<(const CommandAttribute &rhs) const {
        if (type < rhs.type)
            return true;
        if (rhs.type < type)
            return false;
        if (abilityId < rhs.abilityId)
            return true;
        if (rhs.abilityId < abilityId)
            return false;
        switch (type) {
            case TARGETED:
                return std::get<const sc2::Unit*>(target) < std::get<const sc2::Unit*>(rhs.target);
            case POSITIONAL:
                return comparePositions(std::get<CCPosition>(target), std::get<CCPosition>(rhs.target));
            case NONE:
                return false;
        }
        return false;
    }

    bool CommandAttribute::operator>(const CommandAttribute &rhs) const {
        return rhs < *this;
    }

    bool CommandAttribute::operator<=(const CommandAttribute &rhs) const {
        return !(rhs < *this);
    }

    bool CommandAttribute::operator>=(const CommandAttribute &rhs) const {
        return !(*this < rhs);
    }

    bool CommandAttribute::operator==(const CommandAttribute &rhs) const {
        return type == rhs.type &&
               abilityId == rhs.abilityId &&
               target == rhs.target;
    }

    bool CommandAttribute::operator!=(const CommandAttribute &rhs) const {
        return !(rhs == *this);
    }

    CommandAttribute::CommandAttribute(const CommandAttribute::Type type, sc2::ABILITY_ID abilityId,
                                       const TargetType &target) : type(type),
                                                                                               abilityId(abilityId),
                                                                                               target(target) {}

    CommandAttribute::CommandAttribute(const CommandAttribute& rhs)
            : type(rhs.type)
            , abilityId(rhs.abilityId)
            , target(rhs.target) {
    }

    CommandAttribute::CommandAttribute() {}

    Command::Command(const CommandAttribute::Type type, sc2::ABILITY_ID abilityId,
                     const TargetType &target, const std::vector<const sc2::Unit *> &issuers)
            : CommandAttribute(type, abilityId, target), issuers(issuers) {}

    IssuedCommand::IssuedCommand() {}
} // namespace NPrivate

void UnitCommandManager::issueAllCommands(int frameId) {
    // TODO: remove std map?
    std::map<NPrivate::CommandAttribute, std::vector<const sc2::Unit*>> groupedCommands;
    for (auto& x : commands) {
        auto& it = groupedCommands[x];
        for (auto& y : x.issuers) {
            auto& lastCmd = lastIssuedCommand[y->tag];
            if (lastCmd == x) {
                if (lastCmd.frameId + FRAME_SKIP_THRESHOLD > frameId) {
                    continue;
                }
            }
            it.push_back(y);
            static_cast<NPrivate::CommandAttribute&>(lastCmd) = x;
            lastCmd.frameId = frameId;
        }
    }

    for (auto& lr : groupedCommands) {
        auto&& attribute = lr.first;
        auto&& units = lr.second;
        switch (attribute.type) {
            case NPrivate::CommandAttribute::TARGETED:
                m_bot.Actions()->UnitCommand(
                        units,
                        attribute.abilityId,
                        std::get<const sc2::Unit*>(attribute.target)
                );
                break;
            case NPrivate::CommandAttribute::POSITIONAL:
                m_bot.Actions()->UnitCommand(
                        units,
                        attribute.abilityId,
                        std::get<CCPosition>(attribute.target)
                );
                break;
            case NPrivate::CommandAttribute::SELF_TARGET:

            case NPrivate::CommandAttribute::NONE:
                break;
        }
    }
    commands.clear();
}

UnitCommandManager::UnitCommandManager(CCBot &bot)
    : m_bot(bot)
{

}

void UnitCommandManager::UnitCommand(const sc2::Unit* unit, sc2::AbilityID ability, const CCPosition &point) {
    commands.push_back(NPrivate::Command{
            NPrivate::Command::POSITIONAL,
            ability,
            point,
            {unit}
    });
}

void UnitCommandManager::UnitCommand(const sc2::Unit* unit, sc2::AbilityID ability, const sc2::Unit *target) {
    commands.push_back(NPrivate::Command{
            NPrivate::Command::POSITIONAL,
            ability,
            target,
            {unit}
    });
}

void UnitCommandManager::UnitCommand(const sc2::Unit* unit, sc2::AbilityID ability) {
    commands.push_back(NPrivate::Command{
            NPrivate::Command::SELF_TARGET,
            ability,
            std::monostate(),
            {unit}
    });
}
