#include "eventQueue.h"

namespace pho {
///*
void EventQueue::push(boost::array<float,7> tEvent ) {
    serialQueue.push_front(tEvent);
}
//*/

bool EventQueue::isSerialEmpty() {
    return serialQueue.empty();
}


boost::array<float, 7> EventQueue::serialPop() {
    boost::array<float,7> temp;
    temp = serialQueue.front(); //serialQueue‚ÌÅ‰‚Ì—v‘f‚Ö‚ÌQÆ‚ğtemp‚Ö“n‚·
    serialQueue.pop_front(); //serialQueue‚Ì—v‘f‚ğíœ
    return temp;
}

}