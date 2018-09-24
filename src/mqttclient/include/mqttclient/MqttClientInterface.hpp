#pragma once

#include <string>
#include <memory>

#include <boost/function.hpp>

#include <mosquitto.h>
#include <mosquittopp.h>

namespace Mqtt
{

class CMqttClientInterface : public mosq::mosquittopp
{
    public:

        explicit CMqttClientInterface();
        virtual ~CMqttClientInterface();

        void startMqttConnection(std::string host, int server_port);
        void stopMqttConnection();

        void setTopic(const std::string& topic, const std::string& msg);
        void subscribeTopic(const std::string& topic);

    private:
        void on_connect(int rc) override;
        void on_message(const struct mosquitto_message *message) override;
        void on_disconnect(int rc) override;
        void on_error() override;
};

} //namespace CapiMqtt

