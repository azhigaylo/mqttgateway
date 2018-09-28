#include "mqttclient/MqttClientInterface.hpp"

#include <string>
#include <iostream>

#include "common/slog.h"

namespace Mqtt
{

CMqttClientInterface::CMqttClientInterface()
    : mosquittopp("mqtt.client")
    , m_connection_state(false)
{
    printInfo("CMqttClientInterface/%s: created", __FUNCTION__);

}

CMqttClientInterface::~CMqttClientInterface()
{
    printInfo("CMqttClientInterface/%s  was deleted", __FUNCTION__);
}

void CMqttClientInterface::startMqttConnection(const std::string& host, int server_port, const std::string& user, const std::string& pwd)
{
    printInfo("CMqttClientInterface/%s: starting...", __FUNCTION__);

    // start mosquitto internal loop
    mosqpp::lib_init();
    loop_start();

    // set credentials
    if (!user.empty() && !pwd.empty())
    {
        username_pw_set(user.c_str(), pwd.c_str());
    }

    // connect to mqtt
    connect(host.c_str(), server_port, 120);

    printInfo("CMqttClientInterface/%s: started", __FUNCTION__);
}

void CMqttClientInterface::stopMqttConnection()
{
    printInfo("CMqttClientInterface/%s: stopping...", __FUNCTION__);

    // stop mosquitto internal loop
    loop_stop(true);
    mosqpp::lib_cleanup();

    printInfo("CMqttClientInterface/%s: stopped", __FUNCTION__);
}

void CMqttClientInterface::setTopic(const std::string& topic, const std::string& msg)
{
    printDebug("CMqttClientInterface/%s: send msg: '%s' to topic: %s ", __FUNCTION__, msg.c_str(), topic.c_str());

    publish(NULL, topic.c_str(), static_cast<int>(msg.size()), static_cast<const void*>(msg.c_str()), 1, true);
}

void CMqttClientInterface::subscribeTopic(const std::string& topic)
{
    printDebug("CMqttClientInterface/%s: subscribe on topic: %s", __FUNCTION__, topic.c_str());

    subscribe(NULL, topic.c_str(), 1);
}


void CMqttClientInterface::on_connect(int rc)
{
    if (0 == rc)
    {
        printDebug("CMqttClientInterface/%s:connected to mqtt broker", __FUNCTION__);
        m_connection_state = true;
        m_sig_mqtt_connection(true);
    }
    else
    {
        printWarning("CMqttClientInterface/%s:connection error to mqtt broker", __FUNCTION__);
        m_connection_state = false;
        m_sig_mqtt_connection(false);
    }
}

void CMqttClientInterface::on_disconnect(int /*rc*/)
{
    printWarning("CMqttClientInterface/%s: lost communication with mqtt brocker", __FUNCTION__);

    m_connection_state = false;
    m_sig_mqtt_connection(false);
}

void CMqttClientInterface::on_error()
{
    printWarning("CMqttClientInterface/%s: lost communication with mqtt brocker", __FUNCTION__);

    m_connection_state = false;
    m_sig_mqtt_connection(false);
}

void CMqttClientInterface::on_message(const struct mosquitto_message* message)
{
    m_sig_topic_update(std::string((char*)message->topic), std::string((char*)message->payload));
}

} //namespase CapiMqtt
