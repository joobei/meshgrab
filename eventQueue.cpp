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
    temp = serialQueue.front(); //serialQueue�̍ŏ��̗v�f�ւ̎Q�Ƃ�temp�֓n��
    serialQueue.pop_front(); //serialQueue�̗v�f���폜
    return temp;
}

}