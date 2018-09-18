#pragma once

#include <string>

namespace Parsers
{

class CConfigParser final
{
    public:
        explicit CConfigParser(const std::string& config_path);
        ~CConfigParser();

        const std::string& getDataIP()       const {return m_data_server_ip;}
        uint32_t getDataPort()               const {return m_data_server_port;}
        const std::string& getDataUserName() const {return m_data_server_username;}
        const std::string& getDataPwd()      const {return m_data_server_password;}

        const std::string& getBrokerIP()       const {return m_mqtt_broker_ip;}
        uint32_t getBrokerPort()               const {return m_mqtt_broker_port;}
        const std::string& getBrokerUserName() const {return m_mqtt_broker_username;}
        const std::string& getBrokerPwd()      const {return m_mqtt_broker_password;}

    private:

        CConfigParser(const CConfigParser&) = delete;
        CConfigParser& operator=(const CConfigParser&) = delete;

        std::string m_data_server_ip;
        uint32_t    m_data_server_port;
        std::string m_data_server_username;
        std::string m_data_server_password;

        std::string m_mqtt_broker_ip;
        uint32_t    m_mqtt_broker_port;
        std::string m_mqtt_broker_username;
        std::string m_mqtt_broker_password;
};

} //namespace Parsers

