#pragma once

#include <string>
#include <vector>
#include <utility>

namespace Parsers
{

class CGtwTableParser final
{
    public:

       struct router_item_t
       {
           typedef std::pair <std::uint16_t, std::string> mapp_item_t;

           bool        topic_sub;
           uint32_t    number;
           std::string mqtt_topic;
           std::vector<mapp_item_t> mapping;
       };

       typedef std::pair <std::string, router_item_t> gwt_item_t;

        explicit CGtwTableParser(const std::string& table_path);
        ~CGtwTableParser();

        const std::vector<gwt_item_t>& getGwtTable()const
        {
           return m_gwt_vector;
        }

    private:

        CGtwTableParser(const CGtwTableParser&) = delete;
        CGtwTableParser& operator=(const CGtwTableParser&) = delete;

        std::vector<gwt_item_t> m_gwt_vector;
};

} //namespace Parsers

