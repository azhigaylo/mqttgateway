#include "multidumper/DumperConfig.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "elogger/Logger.hpp"

namespace SLDumper
{

ConfigUnavailableException::ConfigUnavailableException(const std::string& fname)
    : std::runtime_error(std::string("Dumper configuration unavailable: file is not exist: ") + fname )
{}

const std::string CDumperConfig::c_dumper_main_cfg         = "DumperConfig";
const std::string CDumperConfig::c_dumper_main_path        = "dest_path";
const std::string CDumperConfig::c_dumper_main_prefix      = "dump_prefix";

const std::string CDumperConfig::c_dumper_modules_cfg      = "DumperModules";
const std::string CDumperConfig::c_dumper_module_name      = "channel";
const std::string CDumperConfig::c_dumper_modules_enable   = "enable";
const std::string CDumperConfig::c_dumper_modules_prio     = "priority";
const std::string CDumperConfig::c_dumper_modules_location = "external";
const std::string CDumperConfig::c_dumper_modules_opt      = "ModuleOptions";

CDumperConfig::CDumperConfig(const std::string& config_path)
{
    ELOG_NAMED_SCOPE("CDumperConfig");

    if (!boost::filesystem::exists(config_path))
    {
        throw ConfigUnavailableException(config_path);
    }

    namespace pt = boost::property_tree;
    try
    {
        pt::ptree config_ptree;
        pt::read_json(config_path, config_ptree);

        // store dumper option
        pt::ptree &dumper_node = config_ptree.get_child(c_dumper_main_cfg);
        m_dest_path = dumper_node.get<std::string>(c_dumper_main_path);
        m_dump_prefix = dumper_node.get<std::string>(c_dumper_main_prefix);

        ELOG_DBG << __func__ << ": " << "dumper path = "  << m_dest_path.c_str()
                                     << " | dumper prefix = " << m_dump_prefix.c_str();

        // create all dumper's modules
        for (pt::ptree::value_type &module_cfg : config_ptree.get_child(c_dumper_modules_cfg))
        {
            std::string name = module_cfg.second.get<std::string>(c_dumper_module_name);
            std::string prio = module_cfg.second.get<std::string>(c_dumper_modules_prio);
            bool b_enabled   = module_cfg.second.get<bool>(c_dumper_modules_enable);
            bool is_external    = module_cfg.second.get<bool>(c_dumper_modules_location);
            std::string json_opt;
            // store module json
            boost::optional<pt::ptree&> optional_node = module_cfg.second.get_child_optional(c_dumper_modules_opt);
            if(optional_node)
            {
                std::stringstream ss_module_cfg;
                pt::write_json(ss_module_cfg, optional_node.get());
                json_opt = ss_module_cfg.str();
            }
            // set module list
            m_module_cfg.push_back(std::make_shared<CModuleConfig>(name, prio, json_opt, b_enabled, is_external));
            // set module name list
            m_module_name_list.push_back(name);
            // set external/internal name list
            if (true == is_external)
            {
                m_ext_module_name_list.push_back(name);
            }
            else
            {
                m_int_module_name_list.push_back(name);
            }
        }

        // store all config to json string, it will be needed to update attribute
        config_ptree.erase(c_dumper_main_cfg);

        std::stringstream ss_common_cfg;
        pt::write_json(ss_common_cfg, config_ptree);
        m_json_cfg = ss_common_cfg.str();
        ELOG_DBG << __func__ << ": " << "Modules config: " << std::endl << m_json_cfg.c_str();
    }
    catch (const ConfigUnavailableException& e)
    {
        ELOG_ERR << __func__ << ": " << "Error description: " << e.what();
        throw;
    }
    catch (const std::exception& e)
    {
        ELOG_ERR << __func__ << ": " << "Error parsing config at path: " << config_path;
        ELOG_ERR << __func__ << ": " << "Error description: " << e.what();
        throw;
    }
}

CDumperConfig::~CDumperConfig()
{
    ELOG_NAMED_SCOPE("CDumperConfig");

    ELOG_DBG << __func__ << ": " << " removed...";
}

CDumperConfig::CModuleConfig::CModuleConfig( const std::string& ch_name,
                                             const std::string& ch_prio,
                                             const std::string& json_opts,
                                             bool  enabled,
                                             bool  external)
    : m_channel_name (ch_name)
    , m_channel_prio (ch_prio)
    , m_json_options (json_opts)
    , m_enabled      (enabled)
    , m_is_external  (external)
{}

CDumperConfig::CModuleConfig::~CModuleConfig()
{
}

const std::string& CDumperConfig::getDumperDestPath() const
{
    return m_dest_path;
}

const std::string& CDumperConfig::getDumperPrefix() const
{
    return m_dump_prefix;
}

const std::string& CDumperConfig::getModulesConfig() const
{
    return m_json_cfg;
}

boost::optional<std::shared_ptr<CDumperConfig::CModuleConfig>> CDumperConfig::getModuleConfig(std::string& channel_name) const
{
    std::vector<std::shared_ptr<CModuleConfig>>::const_iterator find_res = std::find_if(m_module_cfg.begin(), m_module_cfg.end(),
                [&channel_name](const std::shared_ptr<CModuleConfig> &arg) {return arg.get()->m_channel_name == channel_name;});

    return (m_module_cfg.end() != find_res) ? *find_res : boost::optional<std::shared_ptr<CDumperConfig::CModuleConfig>>{};
}

const std::vector<std::string>& CDumperConfig::getModulesNameList() const
{
    return m_module_name_list;
}

const std::vector<std::string>& CDumperConfig::getExtModulesNameList() const
{
    return m_ext_module_name_list;
}

const std::vector<std::string>& CDumperConfig::getIntModulesNameList() const
{
    return m_int_module_name_list;
}

} //namespase SLDumper
