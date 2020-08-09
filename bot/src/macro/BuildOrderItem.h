#pragma once


#include "../general/model/MetaType.h"

struct BuildOrderItem
{
    MetaType        type;		// the thing we want to 'build'
    int             priority;	// the priority at which to place it in the queue
    bool            blocking;	// whether or not we block further items

    BuildOrderItem(const MetaType & t, int p, bool b);
    bool operator<(const BuildOrderItem & x) const;
};
