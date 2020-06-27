#pragma once

#include "../general/model/Common.h"
#include "Squad_L.h"

class CCBot;

class SquadData
{
    CCBot & m_bot;
    std::map<std::string, Squad_L> m_squads;

    void    updateAllSquads();
    void    verifySquadUniqueMembership();

public:

    SquadData(CCBot & bot);

    void            onFrame();
    void            clearSquadData();

    bool            canAssignUnitToSquad(const Unit & unit, const Squad_L & squad) const;
    void            assignUnitToSquad(const Unit & unit, Squad_L & squad);
    void            addSquad(const std::string & squadName, const Squad_L & squad);
    void            removeSquad(const std::string & squadName);
    void            drawSquadInformation();


    bool            squadExists(const std::string & squadName);
    bool            unitIsInSquad(const Unit & unit) const;
    const Squad_L *   getUnitSquad(const Unit & unit) const;
    Squad_L *         getUnitSquad(const Unit & unit);

    Squad_L &         getSquad(const std::string & squadName);
    const std::map<std::string, Squad_L> & getSquads() const;
};
