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

        explicit IGtwItemBase(const Parsers::CGtwTableParser::router_item_t& item)
        : m_router_item(item){}

        Parsers::CGtwTableParser::router_item_t m_router_item;
};
} //namespase Modules

