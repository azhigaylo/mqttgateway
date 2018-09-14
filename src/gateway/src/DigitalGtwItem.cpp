#include "DigitalGtwItem.hpp"

#include <regex>
#include <stdexcept>
#include <boost/bind.hpp>

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
    printDebug("CDigitalGtwItem/%s: was deleted", __FUNCTION__);
}

} //namespase Modules
