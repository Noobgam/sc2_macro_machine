#pragma once
#include <optional>

class CCBot;

class Strategy {
private:
    CCBot & m_bot;

    // you can set gas income goal if you know that you don't need as much gas.
    // e.g. cannon rush will set this goal as zero at the beginning
    // managers should react to strategy goals accordingly by not building geysers
    std::optional<int> m_gasGoal;

    // you can set worker number goal if you plan to cut probes for some reason
    // e.g. cannon rush will set this goal as 16 at the beginning
    std::optional<int> m_workersGoal;

    // you can set worker number of bases goal
    // e.g. cannon rush will set this goal as 1 at the beginning
    // keep in mind that this number is a very bad metric which will make you have 0 income if you do not expand
    std::optional<int> m_expandGoal;
    int m_lastUpdate = 0;
public:
    Strategy(CCBot& bot);

    void onFrame();
    std::optional<int> getGasGoal() const;
    void setGasGoal(std::optional<int> goalO);

    std::optional<int> getWorkersGoal() const;
    void setWorkersGoal(std::optional<int> goalO);

    std::optional<int> getExpandGoal() const;
    void setExpandGoal(std::optional<int> goalO);
};
