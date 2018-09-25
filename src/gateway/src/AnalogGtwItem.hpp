#pragma once

#include <memory>

#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

#include "gateway/GtwItemBase.hpp"
#include "gateway/MqttGatewayImpl.hpp"

namespace Modules
{
class CAnalogGtwItem final : public IGtwItemBase
{
    public:
        typedef boost::signals2::signal<void (const std::string &)> SigTopicSubscribe;
        typedef boost::signals2::signal<void (const std::string &, const std::string )> SigTopicWrire;
        typedef boost::signals2::signal<void (uint32_t, const std::string &)> SigAnalogPointSet;

        typedef std::function<boost::signals2::connection(const MqttGateway::CMqttGatewayImpl::SigAnalogPointUpdate::slot_type &slot)> TAnalogUpdateConnFunc;

        boost::signals2::connection connToSigTopicSubscribe(const SigTopicSubscribe::slot_type &slot)
        {return m_sig_topic_subscribe.connect(slot);}
        boost::signals2::connection connToSigTopicWrire(const SigTopicWrire::slot_type &slot)
        {return m_sig_topic_write.connect(slot);}
        boost::signals2::connection connSigAnalogPointSet(const SigAnalogPointSet::slot_type &slot)
        {return m_sig_analog_poit_set.connect(slot);}

    public:
        CAnalogGtwItem(const Parsers::CGtwTableParser::router_item_t& data);
        virtual ~CAnalogGtwItem() override;

        void initItem(MqttGateway::CMqttGatewayImpl& gtw);

    protected:
        CAnalogGtwItem(const CAnalogGtwItem&) = delete;
        CAnalogGtwItem& operator=(const CAnalogGtwItem&) = delete;

        // gateway <-> gtw item
        void slotAnalogPointUpdate(uint32_t start_poit_num, uint8_t status, double value);
        void slotTopicUpdate(const std::string& topic_name, const std::string& topic_value) override;

    private:
        SigTopicSubscribe  m_sig_topic_subscribe;
        SigTopicWrire      m_sig_topic_write;
        SigAnalogPointSet  m_sig_analog_poit_set;

        boost::signals2::connection m_conn_to_apoint_update;   //connection to analog point update signal from gateway
        boost::signals2::connection m_conn_to_topic_update;    //connection to topic update signal from gateway
};
} //namespase Modules




