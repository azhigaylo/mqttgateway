#include "AnalogGtwItem.hpp"

#include <regex>
#include <stdexcept>
#include <boost/bind.hpp>

#include "common/slog.h"

namespace Modules
{

CAnalogGtwItem::CAnalogGtwItem(const Parsers::CGtwTableParser::router_item_t& data)
    : IGtwItemBase(data)
{
    printDebug("CConfigParser/%s:num = %i / s = %i / topic = %s", __FUNCTION__,
                                                                  data.number,
                                                                  data.topic_sub,
                                                                  data.mqtt_topic.c_str());
}

CAnalogGtwItem::~CAnalogGtwItem()
{
    m_conn_to_apoint_update.disconnect();

    printDebug("CAnalogGtwItem/%s: was deleted", __FUNCTION__);
}

void CAnalogGtwItem::initItem(MqttGateway::CMqttGatewayImpl& gtw)
{
    m_conn_to_apoint_update = gtw.connToSigAnalogPointUpdate(boost::bind(&CAnalogGtwItem::slotAnalogPointUpdate, this,_1, _2, _3));
    m_conn_to_topic_update  = gtw.connToSigTopicUpdate(boost::bind(&CAnalogGtwItem::slotTopicUpdate, this,_1, _2));

    if (true == m_router_item.topic_sub)
    {
       m_sig_topic_subscribe(m_router_item.mqtt_topic);
    }
}

void CAnalogGtwItem::slotAnalogPointUpdate(uint32_t poit_num, uint8_t status, double value)
{
   if (poit_num == m_router_item.number)
   {
        //printDebug("CAnalogGtwItem/%s: I'm analog item[%i], and it's mine !!!", __FUNCTION__, m_router_item.number);

        std::string new_val("unknown");
        if (PointStatus::unknown != status)
        {
            new_val = std::to_string(value);
        }
        m_sig_topic_write(m_router_item.mqtt_topic, new_val);
   }
}

void CAnalogGtwItem::slotTopicUpdate(const std::string& topic_name, const std::string& topic_value)
{
   if (topic_name == m_router_item.mqtt_topic)
   {
      printDebug("CDigitalGtwItem/%s: I'm analog item[%i], and it's my topic !!!", __FUNCTION__, m_router_item.number);

      m_sig_analog_poit_set(m_router_item.number, topic_value);
   }
}

} //namespase Modules
