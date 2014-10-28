#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <deque>
#include <iostream>
#include <boost/array.hpp>

namespace pho {


class EventQueue {
public:
    void push(boost::array<float, 7>);

    bool isSerialEmpty();

    boost::array<float, 7> serialPop();

private:
    std::deque<boost::array<float,7> > serialQueue;
};

}

#endif // EVENTQUEUE_H
