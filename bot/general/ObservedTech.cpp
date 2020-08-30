#include "ObservedTech.h"

bool ObservedTech::addObservedBuff(sc2::BUFF_ID buffId) {
    observedBuffes.insert(buffId);
    if (buffId == sc2::BUFF_ID::DUTCHMARAUDERSLOW) {
        // do something
    }
}
