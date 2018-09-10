#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <atomic>

#include "elogger/Logger.hpp"
#include "elogger/OptionParser.hpp"

#include "gateway/CapiMqttGateway.hpp"
#include "inventory/CapiMqttGtwRevision.hpp"

namespace
{
volatile std::sig_atomic_t is_interrupt = 0;

void signalHandler(int sig)
{
    ELOG_NAMED_SCOPE("CapiMqttMain");

    is_interrupt = sig;

    switch (sig)
    {
    case SIGTERM:
    case SIGINT:
    {
        ELOG_WRN << __func__ << ": " << "Application termination requested [Signal: " << strsignal(sig) << "].";
        break;
    }
    default:
    {
        std::stringstream ss;
        ss << "Application failed. Fatal signal obtained: '" << strsignal(sig) << "'.";
        ELOG_ERR << __func__ << ": " << ss.str();
        _exit(EXIT_FAILURE);
    }
    }
}

void configureSignalHandlers()
{
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}

bool isQuit()
{
    return is_interrupt != 0;
}

} // namespace

int main(int argc, const char** argv)
{
    ELOG_NAMED_SCOPE("CapiMqttMain");

    boost::filesystem::path config_file;

    boost::program_options::options_description desc("CommonAPI-MQTT Gateway Component Options");
    desc.add_options()
        ("help,h", "produce help message")
        ("version,v", "Print the version and exit.")
        ("config,c",
            boost::program_options::value<boost::filesystem::path>(&config_file)->default_value("capimqttgateway.json"),
            std::string("Specify config path. By default: 'capimqttgateway.json'").c_str());

    ELogger::Options logopts;
    ELogger::addOptions(desc, logopts);

    boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    }
    catch (const boost::program_options::error& e)
    {
        std::cerr << "Error while parsing: '" << e.what() << "'" << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("version"))
    {
        std::cout << "CommonAPI-MQTT Gateway Component version info:\n" << CapiMqttGtw::getCapiMqttGtwVersion() << std::endl;
        return EXIT_SUCCESS;
    }

    ELogger::init(ELogger::Config(
        ELogger::AppInfo("CMQT", "CommonAPI-MQTT Gateway"),
        ELogger::ContextInfo("MAIN", "CommonAPI-MQTT Gateway main context"),
        logopts));

    ELOG_INF << __func__ << ": " << "CommonAPI-MQTT Gateway Component..";
    ELOG_INF << "CommonAPI-MQTT Gateway revision:\n'" << CapiMqttGtw::getCapiMqttGtwRevision()  << "'.";

    try
    {
        std::unique_ptr<CapiMqtt::CCapiMqttGateway> capi_gateway;
        capi_gateway.reset(new CapiMqtt::CCapiMqttGateway(std::make_shared<CapiMqtt::CConfigParser>(config_file.string())));

        capi_gateway->performStart();

        // Setup signal handlers
        configureSignalHandlers();

        while(!isQuit())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        ELOG_INF << __func__ << ": " <<  "Main routine interrupted. Stopping...";

        // close all activity
        capi_gateway->performStop();

        ELOG_INF << __func__ << ": " << "Main routine stopped";
        ELOG_INF << __func__ << ": " << "Everything cleaned up. Buy Buy...";
    }
    catch (const std::exception& e)
    {
        ELOG_ERR << __func__ << ": " << "Error description: " << e.what();
        ELOG_ERR << __func__ << ": " << "Gateway is closing";
        return 1;
    }

    return EXIT_SUCCESS;
}
