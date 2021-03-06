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
    temp = serialQueue.front(); //serialQueueの最初の要素への参照をtempへ渡す
    serialQueue.pop_front(); //serialQueueの要素を削除
    return temp;
}

}