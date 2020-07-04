#include "ScoutAroundOrder.h"

ScoutAroundOrder::ScoutAroundOrder(CCBot &bot, Squad* squad, CCPosition position) : Order(bot, squad), m_target_position(position) { }