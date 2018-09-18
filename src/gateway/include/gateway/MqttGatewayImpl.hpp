#pragma once

#include <map>
#include <memory>
#include <future>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "GtwItemBase.hpp"
//#include "MqttDataHandler.hpp"
#include "parsers/ConfigParser.hpp"
#include "parsers/GtwTableParser.hpp"
#include "dataclient/DataClientInterface.hpp"

namespace MqttGateway
{

class CMqttGatewayImpl final
{
    typedef std::unique_ptr<Modules::IGtwItemBase> TGtwItemPtr;

    public:
        explicit CMqttGatewayImpl(const std::shared_ptr<Parsers::CConfigParser>   config,
                                  const std::shared_ptr<Parsers::CGtwTableParser> gtw_table);
        virtual ~CMqttGatewayImpl();

        void performStart();
        void performStop();

    protected:
        // gtw item <-> gateway
        void slotTopicSubscribe(const std::string& topic_name);
        void slotTopicWrire(const std::string& topic_name, const std::string& topic_value);
        void slotDigitalPointSet(uint32_t poit_num, uint16_t point_value);
        void slotAnalogPointSet(uint32_t poit_num, const std::string& point_value);
        // HW core <-> gateway
        void slotDigitalPointUpdate(uint32_t start_poit_num, uint32_t number_point);
        void slotAnalogPointUpdate(uint32_t start_poit_num, uint32_t number_point);
        void slotDataConnectionUpdate(bool connection_status);

    private:

        std::shared_ptr<Parsers::CConfigParser>   m_config;
        std::shared_ptr<Parsers::CGtwTableParser> m_gtw_table;
        std::shared_ptr<CDataClientInterface>     m_data_client;

        std::vector<TGtwItemPtr> m_gtw_items;
};

// ------------------------------------------------------------------------------------------------

} // namespace MqttGateway

