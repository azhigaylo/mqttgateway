#include "mqttclient/MqttClientInterface.hpp"

#include <string>
#include <iostream>

namespace Mqtt
{

CMqttClientInterface::CMqttClientInterface()
    : mosquittopp("mqtt.client")
    , m_config(config)
{
    //ELOG_DBG << __func__ << ": " << " created...";
}

CMqttClientInterface::~CMqttClientInterface()
{
    //ELOG_INF << __func__ << ": " << " was deleted";
}

void CMqttClientInterface::startMqttConnection(std::string host, int server_port)
{
    //ELOG_DBG << __func__ << ": " << "starting....";

    // start mosquitto internal loop
    mosqpp::lib_init();
    loop_start();

    connect(host.c_str(), server_port.c_str(), 120);

    //ELOG_DBG << __func__ << ": " << "started";
}

void CMqttClientInterface::stopMqttConnection()
{
    ELOG_DBG << __func__ << ": " << "->";

    // stop mosquitto internal loop
    loop_stop(true);
    mosqpp::lib_cleanup();

    //ELOG_DBG << __func__ << ": " << "<-";
}

void CMqttClientInterface::setTopic(const std::string& topic, const std::string& msg)
{
    //ELOG_DBG << __func__ << ": send msg: " << msg.c_str();

    publish(NULL, topic.c_str(), static_cast<int>(msg.size()), static_cast<const void*>(msg.c_str()));
}

void CMqttClientInterface::on_connect(int rc)
{
    if (0 == rc)
    {
        // ELOG_INF << __func__ << ": connected to " << m_config->getMqttHost() << ":" <<m_config->getMqttPort();
        // subscribe on nuance - > apostera
        // subscribe(NULL, m_config->getMqttTopicFrom().c_str());
    }
    else
    {
        // ELOG_WRN << __func__ << ": connection error  to " << m_config->getMqttHost() << ":" << m_config->getMqttPort();
    }
}

void CMqttClientInterface::on_disconnect(int /*rc*/)
{
    stopMqttConnection();

    throw std::runtime_error(std::string("mqtt brocker disconnected, exit"));
}

void CMqttClientInterface::on_message(const struct mosquitto_message *message)
{
    // m_processor(std::string((char*)message->payload));
}

void CMqttClientInterface::on_error()
{
    stopMqttConnection();

    throw std::runtime_error(std::string("mqtt broker disconnected, exit"));
}

} //namespase CapiMqtt
