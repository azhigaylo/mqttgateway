#include "MqttGatewayImpl.hpp"

#include <vector>
#include <boost/bind.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "common/slog.h"

namespace MqttGateway
{

CMqttGatewayImpl::CMqttGatewayImpl(const std::shared_ptr<Parsers::CConfigParser>   config,
                                   const std::shared_ptr<Parsers::CGtwTableParser> gtw_table)
    : m_config(config)
    , m_gtw_table(gtw_table)
{

    printDebug("CMqttGatewayImpl/%s: created...", __FUNCTION__);
}

CMqttGatewayImpl::~CMqttGatewayImpl()
{
    printDebug("CMqttGatewayImpl/%s: was deleted", __FUNCTION__);
}

void CMqttGatewayImpl::performStart()
{
    try
    {
        // start mqtt -> core SW part
        //m_mqtt_handler = std::make_shared<CMqttDataHandler>(m_config);
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

    printDebug("CMqttGatewayImpl/%s: started...", __FUNCTION__);
}

void CMqttGatewayImpl::performStop()
{
    printDebug("CMqttGatewayImpl/%s: ->", __FUNCTION__);

//    m_mqtt_handler->stopListening();
//    m_mqtt_handler.reset();

    printDebug("CMqttGatewayImpl/%s: <-", __FUNCTION__);
}


} // namespace MqttGateway
