#include "mqttclient/MqttClientInterface.hpp"

#include <string>
#include <iostream>

#include "common/slog.h"

namespace Mqtt
{

CMqttClientInterface::CMqttClientInterface()
    : mosquittopp("mqtt.client")
{
    printDebug("MqttGateway/%s: created", __FUNCTION__);

}

CMqttClientInterface::~CMqttClientInterface()
{
    printDebug("MqttGateway/%s  was deleted", __FUNCTION__);
}

void CMqttClientInterface::startMqttConnection(std::string host, int server_port)
{
    printDebug("MqttGateway/%s: starting...", __FUNCTION__);

    // start mosquitto internal loop
    mosqpp::lib_init();
    loop_start();

    connect(host.c_str(), server_port, 120);

    printDebug("MqttGateway/%s: started", __FUNCTION__);
}

void CMqttClientInterface::stopMqttConnection()
{
    printDebug("MqttGateway/%s: stopping...", __FUNCTION__);

    // stop mosquitto internal loop
    loop_stop(true);
    mosqpp::lib_cleanup();

    printDebug("MqttGateway/%s: stopped", __FUNCTION__);
}

void CMqttClientInterface::setTopic(const std::string& topic, const std::string& msg)
{
    printDebug("MqttGateway/%s: send msg: %s", __FUNCTION__, msg.c_str());

    publish(NULL, topic.c_str(), static_cast<int>(msg.size()), static_cast<const void*>(msg.c_str()));
}

void CMqttClientInterface::on_connect(int rc)
{
    if (0 == rc)
    {
        printDebug("MqttGateway/%s:connected to mqtt broker", __FUNCTION__);
    }
    else
    {
        printWarning("MqttGateway/%s:connection error to mqtt broker", __FUNCTION__);
    }
}

void CMqttClientInterface::on_disconnect(int /*rc*/)
{
    stopMqttConnection();

    throw std::runtime_error(std::string("mqtt brocker disconnected, exit"));
}

void CMqttClientInterface::on_message(const struct mosquitto_message* /*message*/)
{
    // m_processor(std::string((char*)message->payload));
}

void CMqttClientInterface::on_error()
{
    stopMqttConnection();

    throw std::runtime_error(std::string("mqtt broker disconnected, exit"));
}

} //namespase CapiMqtt
