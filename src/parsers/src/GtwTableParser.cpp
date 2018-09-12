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

CGtwTableParser::CGtwTableParser(const std::string& table_path)
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
            //pt::ptree &main_cfg_node = table_ptree.get_child(Tbl::c_gtw_table_main_cfg);

            // create discret point rouring table
            for (pt::ptree::value_type &d_gtw_item : table_ptree.get_child(Tbl::c_gtw_table_d_routing))
            {
                uint32_t item_d_number = static_cast<uint32_t>(d_gtw_item.second.get<unsigned>(Tbl::c_gtw_item_d_number));
                std::string item_topic = d_gtw_item.second.get<std::string>(Tbl::c_gtw_item_topic);
                bool itim_subscription = d_gtw_item.second.get<bool>(Tbl::c_gtw_item_subscription);

                printDebug("CConfigParser/%s: d_num = %i / topic = %s / s = %i", __FUNCTION__, item_d_number,
                                                                                               item_topic.c_str(),
                                                                                               itim_subscription);
            }

            // create analog point rouring table
            for (pt::ptree::value_type &a_gtw_item : table_ptree.get_child(Tbl::c_gtw_table_a_routing))
            {
                uint32_t item_a_number = static_cast<uint32_t>(a_gtw_item.second.get<unsigned>(Tbl::c_gtw_item_a_number));
                std::string item_topic = a_gtw_item.second.get<std::string>(Tbl::c_gtw_item_topic);
                bool itim_subscription = a_gtw_item.second.get<bool>(Tbl::c_gtw_item_subscription);

                printDebug("CConfigParser/%s: a_num = %i / topic = %s / s = %i", __FUNCTION__, item_a_number,
                                                                                               item_topic.c_str(),
                                                                                               itim_subscription);
            }
        }
        catch (const std::exception& e)
        {
            printError("CConfigParser/%s: Error parsing config: %s", __FUNCTION__, table_path.c_str());
            printError("CConfigParser/%s: Error description: %s", __FUNCTION__, e.what());
            throw;
        }
    }
}

CGtwTableParser::~CGtwTableParser()
{
    printDebug("CConfigParser/%s: removed...", __FUNCTION__);
}

} //namespase Parsers
