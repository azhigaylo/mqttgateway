#pragma once

#include "gateway/GtwItemBase.hpp"

#include <memory>

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

namespace Modules
{
class CDigitalGtwItem final : public IGtwItemBase
{
    public:
        typedef boost::signals2::signal<void (const std::string &)> SigTopicSubscribe;
        typedef boost::signals2::signal<void (const std::string &, const std::string )> SigTopicWrire;
        typedef boost::signals2::signal<void (uint32_t, uint16_t)>  SigDigitalPointSet;

        CDigitalGtwItem(const Parsers::CGtwTableParser::router_item_t& data);
        virtual ~CDigitalGtwItem() override;

        void initItem() override;

        boost::signals2::connection connToSigTopicSubscribe(const SigTopicSubscribe::slot_type &slot)
        {return m_sig_topic_subscribe.connect(slot);}
        boost::signals2::connection connToSigTopicWrire(const SigTopicWrire::slot_type &slot)
        {return m_sig_topic_write.connect(slot);}
        boost::signals2::connection connSigDigitalPointSet(const SigDigitalPointSet::slot_type &slot)
        {return m_sig_dig_poit_set.connect(slot);}

    protected:
        CDigitalGtwItem(const CDigitalGtwItem&) = delete;
        CDigitalGtwItem& operator=(const CDigitalGtwItem&) = delete;

    private:
        SigTopicSubscribe  m_sig_topic_subscribe;
        SigTopicWrire      m_sig_topic_write;
        SigDigitalPointSet m_sig_dig_poit_set;
};
} //namespase Modules




