#pragma once

#include <string>
#include "parsers/GtwTableParser.hpp"

namespace Modules
{
class IGtwItemBase
{
    public:
        virtual ~IGtwItemBase() = default;

    protected:
        explicit IGtwItemBase(const Parsers::CGtwTableParser::router_item_t& item) : m_router_item(item){}
        virtual void slotTopicUpdate(const std::string& topic_name, const std::string& topic_value) = 0;

        enum PointStatus
        {
            unknown      = 0,
            reliable     = 1,
            processing   = 16,
            processed    = 32
        };

        Parsers::CGtwTableParser::router_item_t m_router_item;
};
} //namespase Modules

