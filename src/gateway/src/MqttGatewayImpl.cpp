#include "gateway/MqttGatewayImpl.hpp"

#include <vector>
#include <boost/bind.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "common/slog.h"
#include "GtwItemFactory.hpp"
#include "AnalogGtwItem.hpp"
#include "DigitalGtwItem.hpp"

namespace MqttGateway
{

CMqttGatewayImpl::CMqttGatewayImpl(const std::shared_ptr<Parsers::CConfigParser>   config,
                                   const std::shared_ptr<Parsers::CGtwTableParser> gtw_table)
    : m_config(config)
    , m_gtw_table(gtw_table)
    , m_stop_flag(false)
{

    printDebug("CMqttGatewayImpl/%s: created", __FUNCTION__);
}

CMqttGatewayImpl::~CMqttGatewayImpl()
{
    printDebug("CMqttGatewayImpl/%s: was deleted", __FUNCTION__);
}

void CMqttGatewayImpl::performStart()
{
    printDebug("CMqttGatewayImpl/%s: starting...", __FUNCTION__);

    try
    {
        m_stop_flag = false;
        //---------------------start HW core <-> gateway------------
        m_data_client = std::make_shared<CDataClientInterface>(m_gtw_table->getDigitalPointAmount(),
                                                               m_gtw_table->getAnalogPointAmount());
        m_data_client->connToSigDigitalPointUpdate(boost::bind(&CMqttGatewayImpl::slotDigitalPointUpdate, this,_1, _2));
        m_data_client->connSigAnalogPointUpdate(boost::bind(&CMqttGatewayImpl::slotAnalogPointUpdate, this,_1, _2));
        m_data_client->connSigDataConnection(boost::bind(&CMqttGatewayImpl::slotDataConnectionUpdate, this,_1));

        //---------------------mqtt client <-> gateway--------------
        m_mqtt_client = std::make_shared<Mqtt::CMqttClientInterface>();
        m_mqtt_client->connToSigTopicUpdate(boost::bind(&CMqttGatewayImpl::slotTopicUpdate, this,_1, _2));
        m_mqtt_client->connSigMqttConnection(boost::bind(&CMqttGatewayImpl::slotMqttConnectionUpdate, this,_1));

        //---------------------start items <-> gateway------------
        // create gateway items
        CGtwItemFactory& mf = CGtwItemFactory::getModuleFactory();
        const std::vector<Parsers::CGtwTableParser::gwt_item_t>& gtw_table = m_gtw_table->getGwtTable();

        for(auto &gwt_item: gtw_table)
        {
            auto item_ptr = mf.createItem(gwt_item);
            if (item_ptr)
            {
                // test for digital item
                if (nullptr != dynamic_cast<Modules::CDigitalGtwItem*>(item_ptr.get().get()))
                {
                    Modules::CDigitalGtwItem* d_item_ptr = dynamic_cast<Modules::CDigitalGtwItem*>(item_ptr.get().get());

                    d_item_ptr->connToSigTopicSubscribe(boost::bind(&CMqttGatewayImpl::slotTopicSubscribe, this,_1));
                    d_item_ptr->connToSigTopicWrire(boost::bind(&CMqttGatewayImpl::slotTopicWrire, this,_1, _2));
                    d_item_ptr->connSigDigitalPointSet(boost::bind(&CMqttGatewayImpl::slotDigitalPointSet, this,_1, _2));
                }
                // test for analog item
                if (nullptr != dynamic_cast<Modules::CAnalogGtwItem*>(item_ptr.get().get()))
                {
                    Modules::CAnalogGtwItem* a_item_ptr = dynamic_cast<Modules::CAnalogGtwItem*>(item_ptr.get().get());

                    a_item_ptr->connToSigTopicSubscribe(boost::bind(&CMqttGatewayImpl::slotTopicSubscribe, this,_1));
                    a_item_ptr->connToSigTopicWrire(boost::bind(&CMqttGatewayImpl::slotTopicWrire, this,_1, _2));
                    a_item_ptr->connSigAnalogPointSet(boost::bind(&CMqttGatewayImpl::slotAnalogPointSet, this,_1, _2));
                }

                m_gtw_items.push_back(std::move(item_ptr.get()));
            }
        }

        //---------------------start mqtt client----------------------------
        m_mqtt_client->startMqttConnection(m_config->getBrokerIP(), m_config->getBrokerPort(),
                                           m_config->getBrokerUserName(), m_config->getBrokerPwd());
    }
    catch (const std::exception& e)
    {
        printError("CConfigParser/%s: Error description: ", __FUNCTION__, e.what());
        throw;
    }

    printDebug("CMqttGatewayImpl/%s: started", __FUNCTION__);
}

void CMqttGatewayImpl::performStop()
{
    printDebug("CMqttGatewayImpl/%s: ->", __FUNCTION__);

    m_stop_flag = true;

    if (m_data_client)
    {
       m_data_client->stopDataConnection();
       m_data_client.reset();
    }

    if (m_mqtt_client)
    {
        m_mqtt_client->stopMqttConnection();
        m_mqtt_client.reset();
    }

    printDebug("CMqttGatewayImpl/%s: <-", __FUNCTION__);
}

//----------------------data client <-> gateway slots---------------------------------------------------------------------------------------

// data client <-> gateway
void CMqttGatewayImpl::slotDigitalPointUpdate(uint32_t start_poit_num, uint32_t number_point)
{
    for (uint32_t i=start_poit_num; i< start_poit_num+number_point; i++)
    {
        boost::optional<int8_t>  status = m_data_client->getDStatus(i);
        boost::optional<uint16_t> value = m_data_client->getDPoint(i);

        if (status && value)
        {
            printDebug("CMqttGatewayImpl/%s: dpoint[%i](status/value) = %d/%i", __FUNCTION__, i, status.get(), value.get());
           // brodcast to items
            m_sig_digital_update(i, status.get(), value.get());
        }
    }
}

// data client <-> gateway
void CMqttGatewayImpl::slotAnalogPointUpdate(uint32_t start_poit_num, uint32_t number_point)
{
    for (uint32_t i=start_poit_num; i< start_poit_num+number_point; i++)
    {
        boost::optional<int8_t>  status = m_data_client->getAStatus(i);
        boost::optional<double>   value = m_data_client->getAPoint(i);

        if (status && value)
        {
           printDebug("CMqttGatewayImpl/%s: apoint[%i](status/value) = %d/%f", __FUNCTION__, i, status.get(), value.get());
           // brodcast to items
           m_sig_analog_update(i, status.get(), value.get());
        }
    }
}

// data client <-> gateway
void CMqttGatewayImpl::slotDataConnectionUpdate(bool connection_status)
{
    if (true == connection_status)
    {

    }
    else
    {
        if (false == m_stop_flag)
        {
            // stop connection to data server
            m_data_client->stopDataConnection();
            // send start requests
            m_data_reconn_thread.reset(new boost::scoped_thread<>([this](){
                if (true == m_mqtt_client->getConnectionState())
                {
                    boost::this_thread::sleep(boost::posix_time::seconds(timeout::data_timeout_s));
                    printDebug("CMqttGatewayImpl/slotDataConnectionUpdate: reconnection to data server...");
                    if (m_data_client)m_data_client->startDataConnection(m_config->getDataIP(), m_config->getDataPort());
                }
            }));
        }
    }
}

//----------------------mqtt client <-> gateway slots---------------------------------------------------------------------------------------


// mqtt client <-> gateway
void CMqttGatewayImpl::slotTopicUpdate(const std::string& /*topic_name*/, const std::string& /*topic_value*/)
{

}

// mqtt client <-> gateway
void CMqttGatewayImpl::slotMqttConnectionUpdate(bool connection_status)
{
    if (true == connection_status)
    {
        // subscribe for all topics
        for(TGtwItemPtr &gwt_item: m_gtw_items)
        {
            // test for digital item
            if (nullptr != dynamic_cast<Modules::CDigitalGtwItem*>(gwt_item.get()))
            {
                Modules::CDigitalGtwItem* d_item_ptr = dynamic_cast<Modules::CDigitalGtwItem*>(gwt_item.get());
                d_item_ptr->initItem(*this);
            }
            // test for analog item
            if (nullptr != dynamic_cast<Modules::CAnalogGtwItem*>(gwt_item.get()))
            {
                Modules::CAnalogGtwItem* a_item_ptr = dynamic_cast<Modules::CAnalogGtwItem*>(gwt_item.get());
                a_item_ptr->initItem(*this);
            }
        }

        // connect to data server
        m_data_client->startDataConnection(m_config->getDataIP(), m_config->getDataPort());
    }
    else
    {
        if (false == m_stop_flag)
        {
            // stop connection to data server
            m_data_client->stopDataConnection();

            // send start requests
            m_mqtt_reconn_thread.reset(new boost::scoped_thread<>([this](){
                boost::this_thread::sleep(boost::posix_time::seconds(timeout::mqtt_timeout_s));
                printDebug("CMqttGatewayImpl/slotMqttConnectionUpdate: reconnection to mqtt broker...");
                if (m_mqtt_client)m_mqtt_client->reconnectMqttConnection();
            }));
        }
    }
}


//----------------------gtw item <-> gateway slots------------------------------------------------------------------------------------------

// gtw item <-> gateway
void CMqttGatewayImpl::slotTopicSubscribe(const std::string& topic_name)
{
    m_mqtt_client->subscribeTopic(topic_name);
}

// gtw item <-> gateway
void CMqttGatewayImpl::slotTopicWrire(const std::string& topic_name, const std::string& topic_value)
{
    m_mqtt_client->setTopic(topic_name, topic_value);
}

// gtw item <-> gateway
void CMqttGatewayImpl::slotDigitalPointSet(uint32_t poit_num, uint16_t point_value)
{
    m_data_client->setDPoint(poit_num, point_value);
}

// gtw item <-> gateway
void CMqttGatewayImpl::slotAnalogPointSet(uint32_t poit_num, const std::string& point_value)
{
    m_data_client->setAPoint(poit_num, std::stod(point_value));
}


} // namespace MqttGateway
