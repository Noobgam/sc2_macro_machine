#include "BuildOrderItem.h"

BuildOrderItem::BuildOrderItem(const MetaType & t, int p, bool b)
    : type(t)
    , priority(p)
    , blocking(b)
{
}

bool BuildOrderItem::operator < (const BuildOrderItem & x) const
{
    return priority < x.priority;
}