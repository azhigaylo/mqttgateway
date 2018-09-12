#pragma once

#include <string>
#include <utility>      // std::pair, std::make_pair

namespace Parsers
{

class CGtwTableParser final
{
    public:

      std::pair <std::uint16_t, std::string> mapp_item_t;

      struct d_to_t_router_item_t
      {
         std::string mqtt_topic;
         bool topic_sub;
         std::vector<mapp_item_t>mapping;
      };

      struct t_to_d_router_item_t
      {
         std::string mqtt_topic;
         bool topic_sub;
         std::vector<mapp_item_t>mapping;
      };


       product1 = std::make_pair(std::string("lightbulbs"),0.99);



        explicit CGtwTableParser(const std::string& table_path);
        ~CGtwTableParser();

    private:

        CGtwTableParser(const CGtwTableParser&) = delete;
        CGtwTableParser& operator=(const CGtwTableParser&) = delete;

};

} //namespace Parsers

