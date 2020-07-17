#pragma once
#include <general/model/Unit.h>

class CCBot;
class BaseLocation;
class Squad;

class BaseWorkers {
private:
    CCBot & m_bot;
    const BaseLocation * m_baseLocation;

    Squad* m_mineralSquad;
    Squad* m_vespeneSquad;
public:
    BaseWorkers(CCBot & bot, const BaseLocation * base);

    const BaseLocation * getBaseLocation() const;
    const Squad * getMineralSquad() const;
    const Squad * getVespeneSquad() const;

    int getActiveMineralWorkersNumber() const;
    int getIdealMineralWorkersNumber() const;
    int getMaximumMineralWorkersNumber() const;
    int getActiveVespeneWorkers() const;
    int getIdealVespeneWorkers() const;
    int getMaximumVespeneWorkers() const;

    double getMineralIncome() const;
    double getVespeneIncome() const;

    void assignToMineral(const Unit* unit);
    void assignToVespene(const Unit* unit);

    void deform();
};
