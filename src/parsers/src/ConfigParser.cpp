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
	const std::string c_mqtt_opt             = "MqttClientConfig";
	const std::string c_mqtt_broker_ip       = "broker_ip";
	const std::string c_mqtt_broker_port     = "broker_port";
	const std::string c_mqtt_broker_username = "username";
	const std::string c_mqtt_broker_password = "password";
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
            pt::ptree &opt_node = config_ptree.get_child(Cfg::c_mqtt_opt);

            m_mqtt_broker_ip        = opt_node.get<std::string>(Cfg::c_mqtt_broker_ip);
            m_mqtt_broker_port      = opt_node.get<unsigned>(Cfg::c_mqtt_broker_port);
            m_mqtt_broker_username  = opt_node.get<std::string>(Cfg::c_mqtt_broker_username);
            m_mqtt_broker_password  = opt_node.get<std::string>(Cfg::c_mqtt_broker_password);
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
    printDebug("CConfigParser/%s: removed...", __FUNCTION__);
}

} //namespase Parsers
