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

        // start HW core <-> gateway
        m_data_client = std::make_shared<CDataClientInterface>();
        m_data_client->startDataConnection(std::string("127.0.0.1"), 8096);
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

//    m_mqtt_handler->stopListening();
//    m_mqtt_handler.reset();

    printDebug("CMqttGatewayImpl/%s: <-", __FUNCTION__);
}

void CMqttGatewayImpl::slotTopicSubscribe(const std::string& topic_name)
{
    //m_mqtt_handler->();
    printDebug("CMqttGatewayImpl/%s: need sub for topic '%s'", __FUNCTION__, topic_name.c_str());
}

void CMqttGatewayImpl::slotTopicWrire(const std::string& /*topic_name*/, const std::string& /*topic_value*/)
{

}

void CMqttGatewayImpl::slotDigitalPointSet(uint32_t /*poit_num*/, uint16_t /*point_value*/)
{

}

void CMqttGatewayImpl::slotAnalogPointSet(uint32_t /*poit_num*/, const std::string& /*point_value*/)
{

}

} // namespace MqttGateway
