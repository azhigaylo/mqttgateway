#ifndef DATACLIENTINTERFACE_H
#define DATACLIENTINTERFACE_H

#include <QtCore>
#include <QtNetwork>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/scoped_thread.hpp>

class CDataClientInterface : public QObject
{
    Q_OBJECT

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

    signals:
        void dpoint_updated(int strt_point, int amouint_point);
        void apoint_updated(int strt_point, int amouint_point);
        void dataConnection(bool success);

    public slots:
        void startDataConnection_slot(bool conn_request, QString srv, int port);

    public:
        explicit CDataClientInterface(QObject *parent = 0);
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

        uint32_t getDPointAmount() const {return d_point_total;}
        uint32_t getAPointAmount() const {return a_point_total;}

    private:

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
};

#endif // DATACLIENTINTERFACE_H
