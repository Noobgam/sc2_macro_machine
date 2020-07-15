#pragma once

class CCBot;

class EconomyManager {
private:
    CCBot & m_bot;
public:
    explicit EconomyManager(CCBot & bot);

    double getMineralIncome() const;
    double getVespeneIncome() const;
};
