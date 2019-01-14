/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#pragma once

#include <string>
#include <memory>
#include <atomic>

#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

#include <mosquittopp.h>

namespace Mqtt
{

class CMqttClientInterface : public mosqpp::mosquittopp
{
    public:
        typedef boost::signals2::signal<void (const std::string& topic, const std::string& msg)> SigTopicUpdate;
        typedef boost::signals2::signal<void (bool)>SigMqttConnection;

        boost::signals2::connection connToSigTopicUpdate(const SigTopicUpdate::slot_type &slot)
        {return m_sig_topic_update.connect(slot);}
        boost::signals2::connection connSigMqttConnection(const SigMqttConnection::slot_type &slot)
        {return m_sig_mqtt_connection.connect(slot);}

    public:
        explicit CMqttClientInterface();
        virtual ~CMqttClientInterface();

        void startMqttConnection(const std::string& host, int server_port, const std::string& user, const std::string& pwd);
        void reconnectMqttConnection(){reconnect_async();}
        void stopMqttConnection();
        bool getConnectionState() const {return m_connection_state;}

        void subscribeTopic(const std::string& topic);
        void setTopic(const std::string& topic, const std::string& msg);

    private:
        void on_connect(int rc) override;
        void on_message(const struct mosquitto_message *message) override;
        void on_disconnect(int rc) override;
        void on_error() override;

        std::atomic_bool m_connection_state;
    private:
        SigTopicUpdate    m_sig_topic_update;
        SigMqttConnection m_sig_mqtt_connection;
};

} //namespace CapiMqtt

