#include "parsers/GtwTableParser.hpp"

#include <memory>
#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "common/slog.h"

namespace Parsers
{

namespace Tbl
{
    const std::string c_gtw_table_main_cfg          = "TableConfig";
    const std::string c_gtw_table_d_routing         = "DPointRouting";
    const std::string c_gtw_table_a_routing         = "APointRouting";
    const std::string c_gtw_item_d_number           = "dpoint_number";
    const std::string c_gtw_item_topic              = "mqtt_topic";
    const std::string c_gtw_item_subscription       = "mqtt_subscribe";
    const std::string c_gtw_item_mapping            = "value_mapping";
    const std::string c_gtw_item_mapping_value_int  = "value";
    const std::string c_gtw_item_mapping_value_str  = "mapp_to";
    const std::string c_gtw_item_a_number           = "apoint_number";
}

const std::string& CGtwTableParser::getDigitalKey() {return Tbl::c_gtw_table_d_routing;}
const std::string& CGtwTableParser::getAnalogKey()  {return Tbl::c_gtw_table_a_routing;}

CGtwTableParser::CGtwTableParser(const std::string& table_path)
    : m_digital_max_number (0)
    , m_analog_max_number  (0)
{
    if (!boost::filesystem::exists(table_path))
    {
        throw std::runtime_error(std::string("configuration unavailable: file is not exist: ") + table_path );
    }
    else
    {
        try
        {
            namespace pt = boost::property_tree;
            pt::ptree table_ptree;
            pt::read_json(table_path, table_ptree);

            // gtw options
            // pt::ptree &main_cfg_node = table_ptree.get_child(Tbl::c_gtw_table_main_cfg);

            // create discret point rouring table
            for (pt::ptree::value_type &d_gtw_item : table_ptree.get_child(Tbl::c_gtw_table_d_routing))
            {
                router_item_t router_item;
                router_item.number = static_cast<uint32_t>(d_gtw_item.second.get<unsigned>(Tbl::c_gtw_item_d_number));
                router_item.mqtt_topic = d_gtw_item.second.get<std::string>(Tbl::c_gtw_item_topic);
                router_item.topic_sub = d_gtw_item.second.get<bool>(Tbl::c_gtw_item_subscription);

                printDebug("CGtwTableParser/%s: d_num = %i / s = %i / topic = %s", __FUNCTION__, router_item.number,
                                                                                               router_item.topic_sub,
                                                                                               router_item.mqtt_topic.c_str());
                // create discret point rouring table
                for (pt::ptree::value_type &d_item_mapping : d_gtw_item.second.get_child(Tbl::c_gtw_item_mapping))
                {
                   uint16_t value_int = static_cast<uint32_t>(d_item_mapping.second.get<unsigned>(Tbl::c_gtw_item_mapping_value_int));
                   std::string value_str = d_item_mapping.second.get<std::string>(Tbl::c_gtw_item_mapping_value_str);

                   router_item.mapping.push_back(std::make_pair(value_int, value_str));

                   printDebug("CGtwTableParser/%s: int = %i <-> str = %s", __FUNCTION__, value_int, value_str.c_str());
                }
                digitalCheckForMax(router_item.number);
                m_gwt_vector.push_back(std::make_pair(Tbl::c_gtw_table_d_routing, router_item));
            }

            // create analog point rouring table
            for (pt::ptree::value_type &a_gtw_item : table_ptree.get_child(Tbl::c_gtw_table_a_routing))
            {
                router_item_t router_item;

                router_item.number = static_cast<uint32_t>(a_gtw_item.second.get<unsigned>(Tbl::c_gtw_item_a_number));
                router_item.mqtt_topic = a_gtw_item.second.get<std::string>(Tbl::c_gtw_item_topic);
                router_item.topic_sub  = a_gtw_item.second.get<bool>(Tbl::c_gtw_item_subscription);

                printDebug("CGtwTableParser/%s: a_num = %i / s = %i / topic = %s", __FUNCTION__, router_item.number,
                                                                                               router_item.topic_sub,
                                                                                               router_item.mqtt_topic.c_str());
                analogCheckForMax(router_item.number);
                m_gwt_vector.push_back(std::make_pair(Tbl::c_gtw_table_a_routing, router_item));
            }
        }
        catch (const std::exception& e)
        {
            printError("CGtwTableParser/%s: Error parsing config: %s", __FUNCTION__, table_path.c_str());
            printError("CGtwTableParser/%s: Error description: %s", __FUNCTION__, e.what());
            throw;
        }
    }
}

CGtwTableParser::~CGtwTableParser()
{
    printDebug("CGtwTableParser/%s: removed...", __FUNCTION__);
}

void CGtwTableParser::digitalCheckForMax(uint32_t value)
{
    if (m_digital_max_number < value) m_digital_max_number = value + 1;
}

void CGtwTableParser::analogCheckForMax(uint32_t value)
{
   if (m_analog_max_number < value) m_analog_max_number = value + 1;
}

} //namespase Parsers
