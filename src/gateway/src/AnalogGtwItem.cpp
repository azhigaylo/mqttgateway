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

}

CAnalogGtwItem::~CAnalogGtwItem()
{
    printDebug("CAnalogGtwItem/%s: was deleted", __FUNCTION__);
}
} //namespase Modules
