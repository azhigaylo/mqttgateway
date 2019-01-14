/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "GtwItemFactory.hpp"

#include "common/slog.h"
#include "AnalogGtwItem.hpp"
#include "DigitalGtwItem.hpp"

namespace MqttGateway
{

CGtwItemFactory& CGtwItemFactory::getModuleFactory()
{
    static CGtwItemFactory instance;
    return instance;
}

boost::optional<std::unique_ptr<Modules::IGtwItemBase>> CGtwItemFactory::createItem(const Parsers::CGtwTableParser::gwt_item_t& item)
{
    // create module
    if (Parsers::CGtwTableParser::getDigitalKey() == item.first)
    {
         return std::unique_ptr<Modules::IGtwItemBase>(new Modules::CDigitalGtwItem(item.second));
    }
    else if (Parsers::CGtwTableParser::getAnalogKey()== item.first)
    {
        return std::unique_ptr<Modules::IGtwItemBase>(new Modules::CAnalogGtwItem(item.second));
    }
    else
    {
        printWarning("CGtwItemFactory/%s: Can't find implementation for item '%s'", __FUNCTION__, item.first.c_str());
    }

    return boost::optional<std::unique_ptr<Modules::IGtwItemBase>>{};
}

} //namespase MqttGateway
