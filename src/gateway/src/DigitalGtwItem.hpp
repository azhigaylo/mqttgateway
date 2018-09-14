#pragma once

#include <memory>

#include "gateway/GtwItemBase.hpp"

namespace Modules
{
class CDigitalGtwItem final : public IGtwItemBase
{
    public:
        CDigitalGtwItem(const Parsers::CGtwTableParser::router_item_t& data);
        virtual ~CDigitalGtwItem() override;

    protected:
        CDigitalGtwItem(const CDigitalGtwItem&) = delete;
        CDigitalGtwItem& operator=(const CDigitalGtwItem&) = delete;

    private:

};
} //namespase Modules




