#pragma once

#include "gateway/GtwItemBase.hpp"

#include <memory>

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

namespace Modules
{
class CAnalogGtwItem final : public IGtwItemBase
{
    public:
        typedef boost::signals2::signal<void (const std::string &)> SigTopicSubscribe;
        typedef boost::signals2::signal<void (const std::string &, const std::string )> SigTopicWrire;
        typedef boost::signals2::signal<void (uint32_t, const std::string &)> SigAnalogPointSet;

        CAnalogGtwItem(const Parsers::CGtwTableParser::router_item_t& data);
        virtual ~CAnalogGtwItem() override;

        void initItem() override;

        boost::signals2::connection connToSigTopicSubscribe(const SigTopicSubscribe::slot_type &slot)
        {return m_sig_topic_subscribe.connect(slot);}
        boost::signals2::connection connToSigTopicWrire(const SigTopicWrire::slot_type &slot)
        {return m_sig_topic_write.connect(slot);}
        boost::signals2::connection connSigAnalogPointSet(const SigAnalogPointSet::slot_type &slot)
        {return m_sig_analog_poit_set.connect(slot);}

    protected:
        CAnalogGtwItem(const CAnalogGtwItem&) = delete;
        CAnalogGtwItem& operator=(const CAnalogGtwItem&) = delete;

    private:
        SigTopicSubscribe  m_sig_topic_subscribe;
        SigTopicWrire      m_sig_topic_write;
        SigAnalogPointSet  m_sig_analog_poit_set;
};
} //namespase Modules




