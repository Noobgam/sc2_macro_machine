#pragma once
#include <general/model/Unit.h>
#include <general/managers/resources/Resource.h>

class CCBot;
class Base;
class Squad;

class BaseWorkers {
private:
    CCBot & m_bot;
    const Base * m_base;

    Squad* m_mineralSquad;
    Squad* m_vespeneSquad;
public:
    BaseWorkers(CCBot & bot, const Base * base);

    const Base * getBase() const;

    const Squad * getResourceSquad(ResourceType type) const;
    const Squad * getMineralSquad() const;
    const Squad * getVespeneSquad() const;

    int getActiveResourceWorkers(ResourceType type) const;
    int getActiveMineralWorkers() const;
    int getActiveVespeneWorkers() const;

    int getIdealResourceWorkers(ResourceType type) const;
    int getIdealMineralWorkers() const;
    int getIdealVespeneWorkers() const;

    int getMaximumResourceWorkers(ResourceType type) const;
    int getMaximumMineralWorkers() const;
    int getMaximumVespeneWorkers() const;

    double getResourceIncome(ResourceType type) const;
    double getMineralIncome() const;
    double getVespeneIncome() const;

    void assignToResource(ResourceType type, const Unit* unit);
    void assignToMineral(const Unit* unit);
    void assignToVespene(const Unit* unit);

    void deform();
};
