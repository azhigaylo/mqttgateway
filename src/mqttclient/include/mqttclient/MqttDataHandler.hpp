#pragma once

#include <string>
#include <memory>

#include <boost/function.hpp>

#include <mosquittopp.h>
#include "gateway/ConfigParser.hpp"

namespace CapiMqtt
{

class CMqttDataHandler : public mosqpp::mosquittopp
{
    public:
        typedef std::function<void(const std::string&)> TSendBuildingsInfoFunc;

        explicit CMqttDataHandler(const std::shared_ptr<CConfigParser> config);
        virtual ~CMqttDataHandler();

        void startListening(TSendBuildingsInfoFunc processor);
        void stopListening();

        void sendMessage(const std::string& msg);

    private:
        void on_connect(int rc) override;
        void on_message(const struct mosquitto_message *message) override;

        void on_disconnect(int rc) override;
        void on_error() override;

        std::shared_ptr<CConfigParser> m_config;
        TSendBuildingsInfoFunc m_processor;
};

} //namespace CapiMqtt

