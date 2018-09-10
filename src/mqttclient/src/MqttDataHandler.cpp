
#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "elogger/Logger.hpp"
#include "MqttDataHandler.hpp"

namespace CapiMqtt
{

CMqttDataHandler::CMqttDataHandler(const std::shared_ptr<CConfigParser> config)
    : mosquittopp(config->getMqttClientId().c_str())
    , m_config(config)
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    ELOG_DBG << __func__ << ": " << " created...";
}

CMqttDataHandler::~CMqttDataHandler()
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    ELOG_INF << __func__ << ": " << " was deleted";
}

void CMqttDataHandler::startListening(TSendBuildingsInfoFunc processor)
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    ELOG_DBG << __func__ << ": " << "starting....";

    // store processor
    m_processor = processor;

    // start mosquitto internal loop
    mosqpp::lib_init();
    loop_start();

    connect(m_config->getMqttHost().c_str(), m_config->getMqttPort(), 120);

    ELOG_DBG << __func__ << ": " << "started";
}

void CMqttDataHandler::stopListening()
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    ELOG_DBG << __func__ << ": " << "->";

    // stop mosquitto internal loop
    loop_stop(true);
    mosqpp::lib_cleanup();

    ELOG_DBG << __func__ << ": " << "<-";
}

void CMqttDataHandler::sendMessage(const std::string& msg)
{
    ELOG_DBG << __func__ << ": send msg: " << msg.c_str();

    // send msg apostera -> nuance
    publish(NULL, m_config->getMqttTopicTo().c_str(), static_cast<int>(msg.size()), static_cast<const void*>(msg.c_str()));
}

void CMqttDataHandler::on_connect(int rc)
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    if (0 == rc)
    {
        ELOG_INF << __func__ << ": connected to " << m_config->getMqttHost() << ":" <<m_config->getMqttPort();
        // subscribe on nuance - > apostera
        subscribe(NULL, m_config->getMqttTopicFrom().c_str());
    }
    else
    {
        ELOG_WRN << __func__ << ": connection error  to " << m_config->getMqttHost() << ":" << m_config->getMqttPort();
    }
}

void CMqttDataHandler::on_disconnect(int /*rc*/)
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    stopListening();

    throw std::runtime_error(std::string("mqtt brocker disconnected, exit"));
}

void CMqttDataHandler::on_message(const struct mosquitto_message *message)
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    if (m_processor)
    {
        m_processor(std::string((char*)message->payload));
    }
}

void CMqttDataHandler::on_error()
{
    ELOG_NAMED_SCOPE("CMqttDataHandler");

    stopListening();

    throw std::runtime_error(std::string("mqtt broker disconnected, exit"));
}

} //namespase CapiMqtt
