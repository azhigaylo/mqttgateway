#pragma once

#include <map>
#include <atomic>
#include <memory>
#include <future>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "GtwItemBase.hpp"
#include "parsers/ConfigParser.hpp"
#include "parsers/GtwTableParser.hpp"
#include "dataclient/DataClientInterface.hpp"
#include "mqttclient/MqttClientInterface.hpp"

namespace MqttGateway
{

class CMqttGatewayImpl final
{
    typedef std::unique_ptr<Modules::IGtwItemBase> TGtwItemPtr;

    public:
        typedef boost::signals2::signal<void (uint32_t, uint8_t, uint16_t)> SigDigitalPointUpdate;
        typedef boost::signals2::signal<void (uint32_t, uint8_t, double)> SigAnalogPointUpdate;
        typedef boost::signals2::signal<void (const std::string& topic_name, const std::string& topic_value)> SigTopicUpdate;

        boost::signals2::connection connToSigDigitalPointUpdate(const SigDigitalPointUpdate::slot_type &slot)
        {return m_sig_digital_update.connect(slot);}
        boost::signals2::connection connToSigAnalogPointUpdate(const SigAnalogPointUpdate::slot_type &slot)
        {return m_sig_analog_update.connect(slot);}
        boost::signals2::connection connToSigTopicUpdate(const SigTopicUpdate::slot_type &slot)
        {return m_sig_topic_update.connect(slot);}

        explicit CMqttGatewayImpl(const std::shared_ptr<Parsers::CConfigParser>   config,
                                  const std::shared_ptr<Parsers::CGtwTableParser> gtw_table);
        virtual ~CMqttGatewayImpl();

        void performStart();
        void performStop();

    protected:
        // gtw item <-> gateway
        void slotTopicSubscribe(const std::string& topic_name);
        void slotTopicWrire(const std::string& topic_name, const std::string& topic_value);
        void slotDigitalPointSet(uint32_t poit_num, uint16_t point_value);
        void slotAnalogPointSet(uint32_t poit_num, const std::string& point_value);
        // data client <-> gateway
        void slotDigitalPointUpdate(uint32_t start_poit_num, uint32_t number_point);
        void slotAnalogPointUpdate(uint32_t start_poit_num, uint32_t number_point);
        void slotDataConnectionUpdate(bool connection_status);
        // mqtt client <-> gateway
        void slotTopicUpdate(const std::string& topic_name, const std::string& topic_value);
        void slotMqttConnectionUpdate(bool connection_status);

    private:
        //const uint16_t mqtt_timeout_s = 5;
        //const uint16_t data_timeout_s = 5;
        enum class timeout{mqtt_timeout_s = 5, data_timeout_s = 5};

        std::shared_ptr<Parsers::CConfigParser>    m_config;
        std::shared_ptr<Parsers::CGtwTableParser>  m_gtw_table;
        std::shared_ptr<CDataClientInterface>      m_data_client;
        std::shared_ptr<Mqtt::CMqttClientInterface>m_mqtt_client;

        std::unique_ptr<boost::scoped_thread<>>    m_mqtt_reconn_thread;
        std::unique_ptr<boost::scoped_thread<>>    m_data_reconn_thread;

        std::vector<TGtwItemPtr> m_gtw_items;
        std::atomic_bool         m_stop_flag;

        SigDigitalPointUpdate m_sig_digital_update;
        SigAnalogPointUpdate  m_sig_analog_update;
        SigTopicUpdate        m_sig_topic_update;

};

// ------------------------------------------------------------------------------------------------

} // namespace MqttGateway

