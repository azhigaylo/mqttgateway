#pragma once

#include <string>
#include <memory>

#include <boost/optional.hpp>

#include "GtwItemBase.hpp"
#include "parsers/GtwTableParser.hpp"

namespace MqttGateway
{
class CGtwItemFactory final
{
    public:
        static CGtwItemFactory& getModuleFactory();

        boost::optional<std::shared_ptr<Modules::IGtwItemBase>> createItem(const Parsers::CGtwTableParser::gwt_item_t& item);

    private:
        CGtwItemFactory(){}
        CGtwItemFactory(const CGtwItemFactory& ) = delete;
        CGtwItemFactory& operator=(const CGtwItemFactory& ) = delete;
};
} //namespase MqttGateway
