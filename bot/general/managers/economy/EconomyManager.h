#pragma once

class CCBot;

class EconomyManager {
private:
    CCBot & m_bot;
public:
    explicit EconomyManager(CCBot & bot);

    double getMineralIncome() const;
    double getVespeneIncome() const;

    // returns amount of workerks that may gather minerals on currently occupied bases
    int getMineralWorkersPositions() const;
    // returns amount of workerks that may gather vespene on currently occupied bases
    int getVespeneWorkersPositions() const;
};
