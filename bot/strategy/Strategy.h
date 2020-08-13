#pragma once

class CCBot;

class Strategy {
private:
    CCBot & m_bot;
public:
    Strategy(CCBot& bot);

    void onFrame();
};
