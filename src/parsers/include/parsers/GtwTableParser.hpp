#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include <boost/optional.hpp>

namespace Parsers
{

class ConfigUnavailableException: public std::runtime_error
{
    public:
        ConfigUnavailableException(const std::string& fname);
};

class CConfigParser final
{
    public:
        struct CModuleConfig
        {
            explicit CModuleConfig( const std::string& ch_name,
                                    const std::string& ch_prio,
                                    const std::string& json_opts,
                                    bool  enabled,
                                    bool  external);
            ~CModuleConfig();
            CModuleConfig(const CModuleConfig& ) = delete;
            CModuleConfig& operator=(const CModuleConfig& ) = delete;

            virtual bool operator==(const CModuleConfig& other) const
            {
                return other.m_channel_name == m_channel_name;
            }

            std::string m_channel_name;    // channel name from config, for example "can"/"imu"/...
            std::string m_channel_prio;    // "optional" / "mandatory"
            std::string m_json_options;    //whole json node is stored as string and must be parsed by modules on their own
            bool        m_enabled;
            bool        m_is_external;
        };

    public:
        explicit CConfigParser(const std::string& config_path);
        ~CConfigParser();

        const std::string& getDumperDestPath() const;
        const std::string& getDumperPrefix() const;
        const std::string& getModulesConfig() const;
        boost::optional<std::shared_ptr<CDumperConfig::CModuleConfig>> getModuleConfig(std::string & channel_name) const;
        const std::vector<std::string>& getModulesNameList() const;
        const std::vector<std::string>& getExtModulesNameList() const;
        const std::vector<std::string>& getIntModulesNameList() const;

    private:

        CConfigParser(const CDumperConfig&) = delete;
        CConfigParser& operator=(const CDumperConfig&) = delete;

        std::string m_dest_path;
        std::string m_dump_prefix;
        std::string m_json_cfg;           // "DumperModules" json config, stored as string

        std::vector<std::shared_ptr<CModuleConfig>> m_module_cfg;
        std::vector<std::string> m_module_name_list;
        std::vector<std::string> m_ext_module_name_list;
        std::vector<std::string> m_int_module_name_list;

        // configuration block
        static const std::string c_dumper_main_cfg;
        static const std::string c_dumper_main_path;
        static const std::string c_dumper_main_prefix;
        static const std::string c_dumper_modules_cfg;
        static const std::string c_dumper_module_name;
        static const std::string c_dumper_modules_enable;
        static const std::string c_dumper_modules_prio;
        static const std::string c_dumper_modules_location;
        static const std::string c_dumper_modules_opt;
};
} //namespace SLDumper

