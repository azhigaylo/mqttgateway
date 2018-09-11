#include "MqttGatewayImpl.hpp"

#include <vector>
#include <boost/bind.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>

namespace MqttGateway
{

CMqttGatewayImpl::CMqttGatewayImpl(const std::shared_ptr<Parsers::CConfigParser> config)
    : m_config(config)
{

    ELOG_INF << __func__ << ": " << " started...";
}

CMqttGatewayImpl::~CMqttGatewayImpl()
{
    ELOG_INF << __func__ << ": " << " was deleted";
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

    ELOG_INF << __func__ << ": " << "started";
}

void CMqttGatewayImpl::performStop()
{
    ELOG_INF << __func__ << ": " << "->";

    if (m_pos_listener)
    {
        m_pos_listener->stopListening();
        m_pos_listener.reset();

        m_mqtt_handler->stopListening();
        m_mqtt_handler.reset();
    }

    ELOG_INF << __func__ << ": " << "<-";
}


} // namespace MqttGateway
