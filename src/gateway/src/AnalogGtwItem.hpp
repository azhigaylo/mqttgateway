#pragma once

#include <memory>

#include "gateway/GtwItemBase.hpp"

namespace Modules
{
class CAnalogGtwItem final : public IGtwItemBase
{
    public:
        CAnalogGtwItem(const Parsers::CGtwTableParser::router_item_t& data);
        virtual ~CAnalogGtwItem() override;

    protected:
        CAnalogGtwItem(const CAnalogGtwItem&) = delete;
        CAnalogGtwItem& operator=(const CAnalogGtwItem&) = delete;

    private:

};
} //namespase Modules




