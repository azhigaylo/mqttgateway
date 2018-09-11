#pragma once

#include <string>

namespace Parsers
{

class CConfigParser final
{
    public:
        explicit CConfigParser(const std::string& config_path);
        ~CConfigParser();

        const std::string& getCapId() const {return m_capimqtt_id;}

    private:

        CConfigParser(const CConfigParser&) = delete;
        CConfigParser& operator=(const CConfigParser&) = delete;

        std::string m_capimqtt_id;
        std::string m_capimqtt_positioner_id;

        // configuration block
        static const std::string c_capimqtt_opt;
        static const std::string c_capimqtt_id;
};

} //namespace Parsers

