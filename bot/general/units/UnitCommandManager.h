#pragma once

#include <general/model/Unit.h>

#include <variant>
#include <signal.h>

/**
 * This class is made to reduce the sc2-api load by grouping low-level orders
 *  and to keep track of the last issued command to unit to remove redundant calls to api.
 */

class CCBot;


namespace NPrivate {
    
    using TargetType = std::variant<std::monostate, CCPosition, const sc2::Unit*>;
    
    struct CommandAttribute {
        bool operator<(const CommandAttribute &rhs) const;

        bool operator>(const CommandAttribute &rhs) const;

        bool operator<=(const CommandAttribute &rhs) const;

        bool operator>=(const CommandAttribute &rhs) const;

        bool operator==(const CommandAttribute &rhs) const;

        bool operator!=(const CommandAttribute &rhs) const;

        bool isGroupable() const;

        enum Type {
            // use only for default construction
            NONE = 0,

            POSITIONAL = 1,
            TARGETED = 2,
            SELF_TARGET = 3,
        };

        Type type;
        sc2::ABILITY_ID abilityId;
        TargetType target;

        CommandAttribute(const Type type, sc2::ABILITY_ID abilityId,
                         const TargetType &target);

        CommandAttribute(const CommandAttribute& rhs);

        CommandAttribute();
    };



    struct Command : CommandAttribute {
        std::vector<const sc2::Unit*> issuers;

        Command(const Type type, sc2::ABILITY_ID abilityId, const TargetType &target,
                const std::vector<const sc2::Unit*> &issuers);
    };

    struct IssuedCommand : public CommandAttribute {
        int frameId;

        IssuedCommand();
    };

} // NPrivate

class UnitCommandManager {
private:
    CCBot& m_bot;

    std::vector<NPrivate::Command> commands;
    std::map<sc2::Tag, NPrivate::IssuedCommand> lastIssuedCommand;

    void execute(
        const NPrivate::CommandAttribute& attribute,
        const std::vector<const sc2::Unit*>& units
    );

public:

    explicit UnitCommandManager(CCBot& bot);
    void issueAllCommands(int frameId);

    //! Issues a command to a unit. Targets a point.
    //!< \param unit The unit to send the command to.
    //!< \param ability The ability id of the command.
    //!< \param point The 2D world position to target.
    void UnitCommand(const sc2::Unit* unit, sc2::AbilityID ability, const CCPosition& point);

    //! Issues a command to a unit. Targets another unit.
    //!< \param unit The unit to send the command to.
    //!< \param ability The ability id of the command.
    //!< \param target The unit that is a target of the unit getting the command.
    void UnitCommand(const sc2::Unit* unit, sc2::AbilityID ability, const sc2::Unit* target);

    //! Issues a command to a unit. Self targeting.
    //!< \param unit The unit to send the command to.
    //!< \param ability The ability id of the command.
    void UnitCommand(const sc2::Unit* unit, sc2::AbilityID ability);
};


