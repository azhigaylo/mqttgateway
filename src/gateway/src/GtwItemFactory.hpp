/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <string>
#include <memory>

#include <boost/optional.hpp>

#include "gateway/GtwItemBase.hpp"
#include "parsers/GtwTableParser.hpp"

namespace MqttGateway
{
class CGtwItemFactory final
{
    public:
        static CGtwItemFactory& getModuleFactory();

        boost::optional<std::unique_ptr<Modules::IGtwItemBase>> createItem(const Parsers::CGtwTableParser::gwt_item_t& item);

    private:
        CGtwItemFactory(){}
        CGtwItemFactory(const CGtwItemFactory& ) = delete;
        CGtwItemFactory& operator=(const CGtwItemFactory& ) = delete;
};
} //namespase MqttGateway
