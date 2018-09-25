#pragma once

#include <memory>

#include <boost/bind.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

#include "gateway/GtwItemBase.hpp"
#include "gateway/MqttGatewayImpl.hpp"

namespace Modules
{
class CDigitalGtwItem final : public IGtwItemBase
{
    public:
        typedef boost::signals2::signal<void (const std::string &)> SigTopicSubscribe;
        typedef boost::signals2::signal<void (const std::string &, const std::string )> SigTopicWrire;
        typedef boost::signals2::signal<void (uint32_t, uint16_t)>  SigDigitalPointSet;

        typedef std::function<boost::signals2::connection(const MqttGateway::CMqttGatewayImpl::SigDigitalPointUpdate::slot_type &slot)> TDigitalUpdateConnFunc;

        boost::signals2::connection connToSigTopicSubscribe(const SigTopicSubscribe::slot_type &slot)
        {return m_sig_topic_subscribe.connect(slot);}
        boost::signals2::connection connToSigTopicWrire(const SigTopicWrire::slot_type &slot)
        {return m_sig_topic_write.connect(slot);}
        boost::signals2::connection connSigDigitalPointSet(const SigDigitalPointSet::slot_type &slot)
        {return m_sig_dig_poit_set.connect(slot);}

    public:
        CDigitalGtwItem(const Parsers::CGtwTableParser::router_item_t& data);
        virtual ~CDigitalGtwItem() override;

        void initItem(MqttGateway::CMqttGatewayImpl& gtw);

    protected:
        CDigitalGtwItem(const CDigitalGtwItem&) = delete;
        CDigitalGtwItem& operator=(const CDigitalGtwItem&) = delete;

        // gateway <-> gtw item
        void slotDigitalPointUpdate(uint32_t poit_num, uint8_t status, uint16_t value);
        void slotTopicUpdate(const std::string& topic_name, const std::string& topic_value) override;

    private:
        SigTopicSubscribe  m_sig_topic_subscribe;
        SigTopicWrire      m_sig_topic_write;
        SigDigitalPointSet m_sig_dig_poit_set;

        boost::signals2::connection m_conn_to_dpoint_update;   //connection to digital point update signal from gateway
        boost::signals2::connection m_conn_to_topic_update;    //connection to topic update signal from gateway
};
} //namespase Modules




