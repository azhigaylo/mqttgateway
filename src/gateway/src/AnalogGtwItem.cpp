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

void CAnalogGtwItem::initItem(TAnalogUpdateConnFunc conn_funk)
{
    m_conn_to_apoint_update = conn_funk(boost::bind(&CAnalogGtwItem::slotAnalogPointUpdate, this,_1, _2, _3));

    if (true == m_router_item.topic_sub)
    {
       m_sig_topic_subscribe(m_router_item.mqtt_topic);
    }
}

void CAnalogGtwItem::slotAnalogPointUpdate(uint32_t /*poit_num*/, uint8_t /*status*/, double /*value*/)
{

}

} //namespase Modules
