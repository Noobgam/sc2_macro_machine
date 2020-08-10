#pragma once

class CCBot;

class BoostModule {
private:
    CCBot & m_bot;
public:
    explicit BoostModule(CCBot & bot);

    void onFrame();
};