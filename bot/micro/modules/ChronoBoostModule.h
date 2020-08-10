#pragma once

class CCBot;

class ChronoBoostModule {
private:
    CCBot & m_bot;
public:
    explicit ChronoBoostModule(CCBot & bot);

    void onFrame();
};