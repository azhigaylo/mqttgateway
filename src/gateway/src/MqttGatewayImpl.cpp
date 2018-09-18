#include "gateway/MqttGatewayImpl.hpp"

#include <vector>
#include <boost/bind.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>

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

                item_ptr.get()->initItem();
                m_gtw_items.push_back(std::move(item_ptr.get()));
            }
        }

        //---------------------start HW core <-> gateway------------
        m_data_client = std::make_shared<CDataClientInterface>(m_gtw_table->getDigitalPointAmount(),
                                                               m_gtw_table->getAnalogPointAmount());
        m_data_client->connToSigDigitalPointUpdate(boost::bind(&CMqttGatewayImpl::slotDigitalPointUpdate, this,_1, _2));
        m_data_client->connSigAnalogPointUpdate(boost::bind(&CMqttGatewayImpl::slotAnalogPointUpdate, this,_1, _2));
        m_data_client->connSigDataConnection(boost::bind(&CMqttGatewayImpl::slotDataConnectionUpdate, this,_1));

        m_data_client->startDataConnection(m_config->getDataIP(), m_config->getDataPort());

        //m_mqtt_handler->startListening(std::bind(&MqttGatewayImpl.hpp::publisher, this, std::placeholders::_1));

        // start core -> mqtt SW part
        //m_pos_listener.reset(new CPositionerListener(m_config, m_mqtt_handler));
        //m_pos_listener->startListening();
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

    if (m_data_client)
    {
       m_data_client->stopDataConnection();
       m_data_client.reset();
    }
//    m_mqtt_handler->stopListening();
//    m_mqtt_handler.reset();

    printDebug("CMqttGatewayImpl/%s: <-", __FUNCTION__);
}

// gtw item <-> gateway
void CMqttGatewayImpl::slotTopicSubscribe(const std::string& topic_name)
{
    //m_mqtt_handler->subscribe(topic_name);
    printDebug("CMqttGatewayImpl/%s: need sub for topic '%s'", __FUNCTION__, topic_name.c_str());
}

// gtw item <-> gateway
void CMqttGatewayImpl::slotTopicWrire(const std::string& topic_name, const std::string& topic_value)
{
    //m_mqtt_handler->write(topic_name, topic_value);
    printDebug("CMqttGatewayImpl/%s: topic %s = '%s'", __FUNCTION__, topic_name.c_str(), topic_value.c_str());
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

// HW core <-> gateway
void CMqttGatewayImpl::slotDigitalPointUpdate(uint32_t start_poit_num, uint32_t number_point)
{
    for (uint32_t i=start_poit_num; i< start_poit_num+number_point; i++)
    {
        boost::optional<int8_t>  status = m_data_client->getDStatus(i);
        boost::optional<uint16_t> value = m_data_client->getDPoint(i);

        if (status && value)
        {
            // brodcast to items
            printDebug("CMqttGatewayImpl/%s: dpoint[%i](status/value) = %d/%i", __FUNCTION__, i, status.get(), value.get());
        }
    }
}

// HW core <-> gateway
void CMqttGatewayImpl::slotAnalogPointUpdate(uint32_t start_poit_num, uint32_t number_point)
{
    for (uint32_t i=start_poit_num; i< start_poit_num+number_point; i++)
    {
        boost::optional<int8_t>  status = m_data_client->getAStatus(i);
        boost::optional<double>   value = m_data_client->getAPoint(i);

        if (status && value)
        {
            // brodcast to items
           printDebug("CMqttGatewayImpl/%s: apoint[%i](status/value) = %d/%lf", __FUNCTION__, i, status.get(), value.get());
        }
    }
}

// HW core <-> gateway
void CMqttGatewayImpl::slotDataConnectionUpdate(bool /*connection_status*/)
{

}
} // namespace MqttGateway
