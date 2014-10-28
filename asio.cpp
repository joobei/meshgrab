#include "asio.h"


Minicom_client::Minicom_client(boost::asio::io_service& io_service, unsigned int baud, const std::string& device, EventQueue* queue,boost::mutex* mutex)
    : serialPort(io_service_, device), eventQueue(queue), io_service_(io_service),io_mutex(mutex)
{

    if (!serialPort.is_open())
    {
        std::cout << "Serial Port Failed to open" << std::endl;
        return;
    }
    boost::asio::serial_port_base::baud_rate baud_option(baud);
    serialPort.set_option(baud_option); // set the baud rate after the port has
    serialPort.set_option( boost::asio::serial_port_base::character_size(8));
    serialPort.set_option( boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serialPort.set_option( boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serialPort.set_option( boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    std::string msg = "C";  //command to make polhemus send data constantly
    boost::asio::write(serialPort,boost::asio::buffer(msg.c_str(),msg.size()));

    startRead();

}

///*
void Minicom_client::startRead() {

    //std::string msg = "P";  //command to make polhemus send binary data constantly
    //boost::asio::write(serialPort,boost::asio::buffer(msg.c_str(),msg.size()));

    boost::asio::async_read(serialPort,boost::asio::buffer(serialBuffer,max_read_length),boost::asio::transfer_all(),
                            boost::bind(&Minicom_client::read_complete,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred)
                            );

}
//*/
///*
void Minicom_client::read_complete(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    boost::array<float, 7> topush;

    for (int i=0;i<7;i++) {
        topush[i] = getFloat(i);
    }

    //try {
    boost::mutex::scoped_lock lock(*io_mutex);
    eventQueue->push(topush);
    lock.unlock();

    //send P and wait for more
    startRead();
}


float Minicom_client::getFloat(int index)
{ 	float temp = 0.0;
        memcpy(&temp,serialBuffer+3*sizeof(char)+index*sizeof(float),sizeof(float));
            return temp; }

void Minicom_client::shutDown() {

	std::string msg = "c";  //command to make polhemus stop sending data
    boost::asio::write(serialPort,boost::asio::buffer(msg.c_str(),msg.size()));

}
//*/