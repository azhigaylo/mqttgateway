#include "parsers/ConfigParser.hpp"

#include <memory>
#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "common/slog.h"
#include "common/ptypes.h"

namespace Parsers
{
const std::string CConfigParser::c_capimqtt_opt           = "CapiMqttOptions";
const std::string CConfigParser::c_capimqtt_id            = "capi_id";

CConfigParser::CConfigParser(const std::string& config_path)
{
    ELOG_NAMED_SCOPE("CConfigParser");

    if (!boost::filesystem::exists(config_path))
    {
        throw std::runtime_error(std::string("configuration unavailable: file is not exist: ") + config_path );
    }
    else
    {
        try
        {
            namespace pt = boost::property_tree;
            pt::ptree config_ptree;
            pt::read_json(config_path, config_ptree);
            pt::ptree &opt_node = config_ptree.get_child(c_capimqtt_opt);

            m_capimqtt_id = opt_node.get<std::string>(c_capimqtt_id);

        }
        catch (const std::exception& e)
        {
            printError("CConfigParser/%s: Error parsing config: ", __FUNCTION__, config_path.c_str());
            printError("CConfigParser/%s: Error description: ", __FUNCTION__, e.what());
            throw;
        }
    }
}

CConfigParser::~CConfigParser()
{
    ELOG_NAMED_SCOPE("CConfigParser");

    printDebug("CConfigParser/%s: removed...", __FUNCTION__);
}

} //namespase Parsers
