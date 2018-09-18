#include "parsers/ConfigParser.hpp"

#include <memory>
#include <iostream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "common/slog.h"

namespace Parsers
{

namespace Cfg
{
    const std::string c_data_opt      = "DataClientConfig";
    const std::string c_data_ip       = "data_ip";
    const std::string c_data_port     = "data_port";
    const std::string c_data_username = "data_username";
    const std::string c_data_password = "data_password";

    const std::string c_mqtt_opt             = "MqttClientConfig";
    const std::string c_mqtt_broker_ip       = "mqtt_broker";
    const std::string c_mqtt_broker_port     = "mqtt_broker_port";
    const std::string c_mqtt_broker_username = "mqtt_username";
    const std::string c_mqtt_broker_password = "mqtt_password";
}

CConfigParser::CConfigParser(const std::string& config_path)
{
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

            pt::ptree &opt_node = config_ptree.get_child(Cfg::c_data_opt);
            m_data_server_ip        = opt_node.get<std::string>(Cfg::c_data_ip);
            m_data_server_port      = opt_node.get<unsigned>(Cfg::c_data_port);
            m_data_server_username  = opt_node.get<std::string>(Cfg::c_data_username);
            m_data_server_password  = opt_node.get<std::string>(Cfg::c_data_password);

            opt_node = config_ptree.get_child(Cfg::c_mqtt_opt);
            m_mqtt_broker_ip        = opt_node.get<std::string>(Cfg::c_mqtt_broker_ip);
            m_mqtt_broker_port      = opt_node.get<unsigned>(Cfg::c_mqtt_broker_port);
            m_mqtt_broker_username  = opt_node.get<std::string>(Cfg::c_mqtt_broker_username);
            m_mqtt_broker_password  = opt_node.get<std::string>(Cfg::c_mqtt_broker_password);
        }
        catch (const std::exception& e)
        {
            printError("CConfigParser/%s: Error parsing config: %s", __FUNCTION__, config_path.c_str());
            printError("CConfigParser/%s: Error description: %s", __FUNCTION__, e.what());
            throw;
        }
    }
}

CConfigParser::~CConfigParser()
{
    printDebug("CConfigParser/%s: removed...", __FUNCTION__);
}

} //namespase Parsers
