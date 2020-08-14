#pragma once

class CCBot;

class Strategy {
private:
    CCBot & m_bot;

    int m_lastUpdate = 0;
public:
    Strategy(CCBot& bot);

    void onFrame();
};
