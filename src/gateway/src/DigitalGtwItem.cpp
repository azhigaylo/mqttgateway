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

}

CDigitalGtwItem::~CDigitalGtwItem()
{
    printDebug("CDigitalGtwItem/%s: was deleted", __FUNCTION__);
}
} //namespase Modules
