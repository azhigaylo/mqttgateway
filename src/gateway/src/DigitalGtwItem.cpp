#include "DigitalGtwItem.hpp"

#include <regex>
#include <stdexcept>
#include <boost/function.hpp>

#include "common/slog.h"

namespace Modules
{

CDigitalGtwItem::CDigitalGtwItem(const Parsers::CGtwTableParser::router_item_t& data)
    : IGtwItemBase(data)
{
    printDebug("CConfigParser/%s:num = %i / s = %i / topic = %s", __FUNCTION__,
                                                                  data.number,
                                                                  data.topic_sub,
                                                                  data.mqtt_topic.c_str());
}

CDigitalGtwItem::~CDigitalGtwItem()
{
    m_conn_to_dpoint_update.disconnect();

    printDebug("CDigitalGtwItem/%s: was deleted", __FUNCTION__);
}

void CDigitalGtwItem::initItem(MqttGateway::CMqttGatewayImpl& p_gtw)
{
    m_conn_to_dpoint_update = p_gtw.connToSigDigitalPointUpdate(boost::bind(&CDigitalGtwItem::slotDigitalPointUpdate, this,_1, _2, _3));
    m_conn_to_topic_update  = p_gtw.connToSigTopicUpdate(boost::bind(&CDigitalGtwItem::slotTopicUpdate, this,_1, _2));

    if (true == m_router_item.topic_sub)
    {
       m_sig_topic_subscribe(m_router_item.mqtt_topic);
    }
}

void CDigitalGtwItem::slotDigitalPointUpdate(uint32_t poit_num, uint8_t /*status*/, uint16_t /*value*/)
{
   if (poit_num == m_router_item.number)
   {
      printDebug("CDigitalGtwItem/%s: I'm digital item[%i], and it's mine !!!", __FUNCTION__, m_router_item.number);
   }
}

void CDigitalGtwItem::slotTopicUpdate(const std::string& topic_name, const std::string& /*topic_value*/)
{
   if (topic_name == m_router_item.mqtt_topic)
   {
      printDebug("CDigitalGtwItem/%s: I'm digital item[%i], and it's my topic !!!", __FUNCTION__, m_router_item.number);
   }
}

} //namespase Modules
