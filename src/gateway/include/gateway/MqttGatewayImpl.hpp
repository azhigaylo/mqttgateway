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

    private:

        std::shared_ptr<Parsers::CConfigParser>   m_config;
        std::shared_ptr<Parsers::CGtwTableParser> m_gtw_table;

        std::vector<TGtwItemPtr> m_gtw_items;
};

// ------------------------------------------------------------------------------------------------

} // namespace MqttGateway

