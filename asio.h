#ifndef ASIO_H
#define ASIO_H

#include <boost/asio.hpp>
#include <boost/system/api_config.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "eventQueue.h"
#include <boost/array.hpp>

using namespace pho;

class Minicom_client
{
public:
        Minicom_client(
            boost::asio::io_service& io_service,
            unsigned int baud,
            const std::string& device,
            EventQueue* queue,
            boost::mutex* mutex
            );
		void shutDown();
private:
        void startRead();
        void read_complete(const boost::system::error_code& error, std::size_t bytes_transferred);
        float getFloat(int index);
		

        static const int max_read_length = 7*4+3; // maximum amount of data to read in one operation
        boost::asio::io_service& io_service_;
        boost::asio::serial_port serialPort; // the serial port this instance is connected to
        unsigned char serialBuffer[max_read_length]; // data read from the socket
        EventQueue* eventQueue;
        boost::mutex* io_mutex;
};


#endif // ASIO_H
