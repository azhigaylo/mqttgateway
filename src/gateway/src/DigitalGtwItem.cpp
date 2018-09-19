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

void CDigitalGtwItem::initItem(TDigitalUpdateConnFunc conn_funk)
{
    m_conn_to_dpoint_update = conn_funk(boost::bind(&CDigitalGtwItem::slotDigitalPointUpdate, this,_1, _2, _3));

   if (true == m_router_item.topic_sub)
   {
      m_sig_topic_subscribe(m_router_item.mqtt_topic);
   }
}

void CDigitalGtwItem::slotDigitalPointUpdate(uint32_t /*poit_num*/, uint8_t /*status*/, uint16_t /*value*/)
{

}

} //namespase Modules
