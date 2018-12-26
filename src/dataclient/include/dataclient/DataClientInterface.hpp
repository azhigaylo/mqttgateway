#pragma once

#include <mutex>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/scoped_thread.hpp>

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

class CDataClientInterface final
{
    enum
    {
        d_point_total = 1000,
        a_point_total = 1000
    };

    enum
    {
        GetDiscretPoint    = 1,
        GetAnalogPoint     = 2,
        SetDiscretPoint    = 3,
        SetAnalogPoint     = 4,
        NotifyDiscretPoint = 5,
        NotifyAnalogPoint  = 6
    };

    enum
    {
        magicHeader = 0xAA
    };

    #pragma pack(push, 1)
        struct TDPOINT
        {
           uint16_t number;
           int8_t   status;
           uint16_t value;
        };

        struct TAPOINT
        {
           uint16_t number;
           int8_t   status;
           double   value;
        };

       struct THeader
       {
          uint8_t  head_strt;    // 0xAA
          uint32_t data_size;    // overall package size
          uint8_t  cmd;
          uint16_t start_point;
          uint16_t number_point;
       };

       struct TClientRequest
       {
          THeader header;
          union
          {
             TDPOINT digital;
             TAPOINT analog;
          }point;
       };

       // serve for response or notification
       struct TResponse
       {
          THeader header;
          union
          {
             TDPOINT digital[d_point_total];
             TAPOINT analog[a_point_total];
          }array;
       };

    #pragma pack(pop)

    public:
        typedef boost::signals2::signal<void (uint32_t, uint32_t)> SigDigitalPointUpdate;
        typedef boost::signals2::signal<void (uint32_t, uint32_t)> SigAnalogPointUpdate;
        typedef boost::signals2::signal<void (bool)>SigDataConnection;

        boost::signals2::connection connToSigDigitalPointUpdate(const SigDigitalPointUpdate::slot_type &slot)
        {return m_sig_digital_point_update.connect(slot);}
        boost::signals2::connection connSigAnalogPointUpdate(const SigAnalogPointUpdate::slot_type &slot)
        {return m_sig_analog_point_update.connect(slot);}
        boost::signals2::connection connSigDataConnection(const SigDataConnection::slot_type &slot)
        {return m_sig_data_connection.connect(slot);}

    public:
        explicit CDataClientInterface(uint32_t d_amount, uint32_t a_amount);
        virtual ~CDataClientInterface();

        void startDataConnection(std::string host, int server_port);
        void stopDataConnection();
        bool checkDataConnection()const {return m_connection_state;}

        boost::optional<uint16_t> getDPoint ( uint16_t number );
        boost::optional<double>   getAPoint ( uint16_t number );

        boost::optional<int8_t>   getDStatus( uint16_t number );
        boost::optional<int8_t>   getAStatus( uint16_t number );

        bool setDPoint ( uint16_t number, uint16_t value );
        bool setAPoint ( uint16_t number, double value );

    private:

        CDataClientInterface(const CDataClientInterface&) = delete;
        CDataClientInterface& operator=(const CDataClientInterface&) = delete;

        void connectionHandler(const boost::system::error_code& error);
        void readHandler(const boost::system::error_code& error, size_t bytes_transferred);
        void processIncoming();
        void updateConnectionState(bool state);

        void sendGetDpointsReq(uint16_t start_point, uint16_t number_point);
        void sendGetApointsReq(uint16_t start_point, uint16_t number_point);

        void sendSetDpointReq(uint16_t point_number, const TDPOINT& value);
        void sendSetApointReq(uint16_t point_number, const TAPOINT& value);

        void serviceLoop();

        TAPOINT APOINT[a_point_total];
        TDPOINT DPOINT[d_point_total];

        std::mutex m_dpoint_mtx;
        std::mutex m_apoint_mtx;
        std::unique_ptr<boost::scoped_thread<>>        m_comm_thread;
        std::unique_ptr<boost::scoped_thread<>>        m_srv_thread;             // thread where io_service will run
        boost::asio::io_service                        m_io_service;             // input/output instance
        std::unique_ptr<boost::asio::ip::tcp::socket>  m_boost_socket;           // socket port object
        std::shared_ptr<boost::asio::io_service::work> m_io_dummy_work;

        boost::asio::streambuf::mutable_buffers_type   m_raw_buf;
        boost::asio::streambuf                         m_read_buffer;

        std::atomic_bool                               m_connection_state;
        uint32_t                                       m_a_point_amount;
        uint32_t                                       m_d_point_amount;

    private:
        SigDigitalPointUpdate  m_sig_digital_point_update;
        SigAnalogPointUpdate   m_sig_analog_point_update;
        SigDataConnection      m_sig_data_connection;
};
