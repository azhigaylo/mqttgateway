#include "dataclient/DataClientInterface.hpp"

#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

#include "common/slog.h"

CDataClientInterface::CDataClientInterface(uint32_t d_amount, uint32_t a_amount)
   : m_raw_buf(m_read_buffer.prepare(0))
   , m_connection_state(false)
   , m_a_point_amount(a_amount)
   , m_d_point_amount(d_amount)
{
    m_io_dummy_work = std::make_shared<boost::asio::io_service::work>(m_io_service);
    m_srv_thread.reset(new boost::scoped_thread<>(boost::bind(&CDataClientInterface::serviceLoop, this)));
    m_boost_socket.reset(new boost::asio::ip::tcp::socket(m_io_service));
}

CDataClientInterface::~CDataClientInterface()
{
    m_io_service.stop();
    m_srv_thread->join();
    printInfo("CDataClientInterface/%s: deleted.", __FUNCTION__);
}

void CDataClientInterface::startDataConnection(std::string host, int server_port)
{
    try
    {
        boost::asio::ip::tcp::endpoint server;
        server.address(boost::asio::ip::address::from_string(host));
        server.port(server_port);

        printInfo("CDataClientInterface/%s: : try to connect to '%s:%i'", __FUNCTION__, host.c_str(), server_port);

        m_boost_socket->async_connect(server, boost::bind(&CDataClientInterface::connectionHandler, this, boost::asio::placeholders::error));
    }
    catch(boost::system::system_error e)
    {
       printError("CDataClientInterface/%s: error = %i", __FUNCTION__, e.code());
    }
}

void CDataClientInterface::stopDataConnection()
{
    m_boost_socket->cancel();
    m_boost_socket->close();

    printDebug("CDataClientInterface/%s: socket was closed.", __FUNCTION__);
}

// --------------------------------------------------------

boost::optional<uint16_t> CDataClientInterface::getDPoint( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_dpoint_mtx);

    return (true == m_boost_socket->is_open()) ? DPOINT[number].value : boost::optional<uint16_t>{};
}

boost::optional<double> CDataClientInterface::getAPoint( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_apoint_mtx);

    return (true == m_boost_socket->is_open()) ? APOINT[number].value : boost::optional<double>{};
}

boost::optional<int8_t> CDataClientInterface::getDStatus( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_dpoint_mtx);

    return (true == m_boost_socket->is_open()) ? DPOINT[number].status : boost::optional<int8_t>{};
}

boost::optional<int8_t> CDataClientInterface::getAStatus( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_apoint_mtx);

    return (true == m_boost_socket->is_open()) ? APOINT[number].status : boost::optional<int8_t>{};
}


bool CDataClientInterface::setDPoint( uint16_t number, uint16_t value )
{
    bool result = false;
    TDPOINT d_point = {number, 16, value};

    if (true == m_boost_socket->is_open())
    {
        if (value != DPOINT[number].value)
        {
           sendSetDpointReq(number, d_point);
        }
        result = true;
    }
    return result;
}

bool CDataClientInterface::setAPoint( uint16_t number, double value )
{
    bool result = false;
    TAPOINT a_point = {number, 16, value};

    if (true == m_boost_socket->is_open())
    {
        if (value != APOINT[number].value)
        {
           sendSetApointReq(number, a_point);
        }
        result = true;
    }
    return result;
}

// --------------------------------------------------------

void CDataClientInterface::connectionHandler(const boost::system::error_code& error)
{
    if (boost::system::errc::success == error)
    {
        printInfo("CDataClientInterface/%s: connected...", __FUNCTION__);

        // allocate new subbuffer & wait for response package
        m_raw_buf = m_read_buffer.prepare(sizeof(TResponse));

        // wait for read request package
        m_boost_socket->async_read_some(m_raw_buf, boost::bind(&CDataClientInterface::readHandler,
                                                               this,
                                                               boost::asio::placeholders::error,
                                                               boost::asio::placeholders::bytes_transferred));
        // send start requests
        m_comm_thread.reset(new boost::scoped_thread<>([this](){ sendGetDpointsReq(0, m_d_point_amount);
                                                                 std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                                                 sendGetApointsReq(0, m_a_point_amount);
                                                                 std::this_thread::sleep_for(std::chrono::milliseconds(10));}));
        updateConnectionState(true);
    }
    else
    {
       printError("CDataClientInterface/%s: error(%i), message: %s", __FUNCTION__, error.value(), error.message().c_str());
       updateConnectionState(false);
    }
}

void CDataClientInterface::readHandler(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(boost::system::errc::success == error.value())
    {
        m_read_buffer.commit(bytes_transferred);

        processIncoming();

        // allocate new subbuffer & wait for response package
        m_raw_buf = m_read_buffer.prepare(sizeof(TResponse));
        m_boost_socket->async_read_some(m_raw_buf, boost::bind(&CDataClientInterface::readHandler,
                                                               this,
                                                               boost::asio::placeholders::error,
                                                               boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        switch (error.value())
        {
            // operation canceled - it's ok for us
            case boost::system::errc::operation_canceled :
            {
                updateConnectionState(false);
                break;
            }
            case boost::system::errc::connection_reset:
            case boost::system::errc::no_such_file_or_directory :
            {
                updateConnectionState(false);
                break;
            }
            default :
            {
                printError("CDataClientInterface/%s: error(%i), message: %s", __FUNCTION__, error.value(), error.message().c_str());
                break;
            }
        }
    }
}

void CDataClientInterface::processIncoming()
{
    std::istream is_data(&m_read_buffer);

    // check magic header 'magicHeader'
    if (magicHeader == is_data.peek())
    {
        while(true)
        {
            // read header
            TResponse package;
            is_data.read(reinterpret_cast<char*>(&package.header), sizeof(package.header));
            if ((static_cast<std::size_t>(is_data.gcount())) >= sizeof(package.header))
            {
                // read data
                is_data.read(reinterpret_cast<char*>(&package.array), package.header.data_size - sizeof(package.header));
                if ((static_cast<std::size_t>(is_data.gcount())) >=  package.header.data_size - sizeof(package.header))
                {
                    switch(package.header.cmd)
                    {
                      case GetDiscretPoint :
                      {
                         {
                            std::lock_guard<std::mutex> lock(m_dpoint_mtx);
                            for (int i=0; i < package.header.number_point; i++)
                            {
                               DPOINT[package.header.start_point + i] = package.array.digital[i];
                            }
                         }
                         m_sig_digital_point_update(package.header.start_point, package.header.number_point);
                         break;
                      }
                      case GetAnalogPoint :
                      {
                         {
                            std::lock_guard<std::mutex> lock(m_apoint_mtx);
                            for (int i=0; i < package.header.number_point; i++)
                            {
                               APOINT[package.header.start_point + i] = package.array.analog[i];
                            }
                         }
                         m_sig_analog_point_update(package.header.start_point, package.header.number_point);
                         break;
                      }
                      case NotifyDiscretPoint :
                      {
                         {
                            std::lock_guard<std::mutex> lock(m_dpoint_mtx);
                            for (int i=0; i < package.header.number_point; i++)
                            {
                               DPOINT[package.header.start_point + i] = package.array.digital[i];
                            }
                         }
                         m_sig_digital_point_update(package.header.start_point, package.header.number_point);
                         break;
                      }
                      case NotifyAnalogPoint :
                      {
                         {
                            std::lock_guard<std::mutex> lock(m_apoint_mtx);
                            for (int i=0; i < package.header.number_point; i++)
                            {
                               APOINT[package.header.start_point + i] = package.array.analog[i];
                            }
                         }
                         m_sig_analog_point_update(package.header.start_point, package.header.number_point);
                         break;
                      }
                      default :
                      {
                          printWarning("CDataClientInterface/%s: unknown command = %i", __FUNCTION__, package.header.cmd);
                          break;
                      }
                    }
                    m_read_buffer.consume(package.header.data_size);
                }
                else break;
            }
            else break;
        }
    }
    else
    {
        printWarning("CDataClientInterface/%s: bad package, skip it...", __FUNCTION__);
        m_read_buffer.consume(m_read_buffer.size());
        throw;
    }
}

void CDataClientInterface::updateConnectionState(bool state)
{
    m_connection_state = state;
    m_sig_data_connection(m_connection_state);
}

void CDataClientInterface::sendGetDpointsReq(uint16_t start_point, uint16_t number_point)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(THeader);
    m_request.header.cmd          = GetDiscretPoint;
    m_request.header.start_point  = start_point;
    m_request.header.number_point = number_point;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::sendGetApointsReq(uint16_t start_point, uint16_t number_point)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(THeader);
    m_request.header.cmd          = GetAnalogPoint;
    m_request.header.start_point  = start_point;
    m_request.header.number_point = number_point;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::sendSetDpointReq(uint16_t point_number, const TDPOINT& value)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(m_request);
    m_request.header.cmd          = SetDiscretPoint;
    m_request.header.start_point  = point_number;
    m_request.header.number_point = 1;
    m_request.point.digital = value;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::sendSetApointReq(uint16_t point_number, const TAPOINT& value)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(m_request);
    m_request.header.cmd          = SetAnalogPoint;
    m_request.header.start_point  = point_number;
    m_request.header.number_point = 1;
    m_request.point.analog = value;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::serviceLoop()
{
    try
    {
        m_io_service.run();
        printDebug("CDataClientInterface/%s: io service shutdown...", __FUNCTION__);
    }
    catch(const std::exception& e)
    {
       printWarning("CDataClientInterface/%s: io service Err: %s", __FUNCTION__, e.what());
    }
}
