#include <atomic>
#include <thread>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "common/slog.h"
#include "gateway/MqttGatewayImpl.hpp"

namespace
{
volatile std::sig_atomic_t is_interrupt;

void signalHandler(int sig)
{
    is_interrupt = sig;

    switch (sig)
    {
    case SIGTERM:
    case SIGINT:
    {
        printDebug("MqttGateway/%s: Application termination requested [Signal:%i]", __FUNCTION__, sig);
        break;
    }
    default:
    {
        printError("MqttGateway/%s: Fatal signal obtained: [Signal:%i], exit...", __FUNCTION__, sig);
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
    uint32_t debug_level;
    uint32_t debug_sink;
    boost::filesystem::path config_file;
    boost::filesystem::path gtw_table_file;

    boost::program_options::options_description desc("HomeBrain core <-> MQTT Gateway Component Options");
    desc.add_options()
        ("help,h", "produce help message")
        ("debug,d", boost::program_options::value<uint32_t>(&debug_level), "debug level 0-4(err/wr/info/dbg")
        ("sink,s",  boost::program_options::value<uint32_t>(&debug_sink), "debug sink 0-1(console/dlt")
        ("config,c",boost::program_options::value<boost::filesystem::path>(&config_file)->default_value("gtw_table.json"),
                    std::string("Specify gatway table path. By default: 'gtw_table.json'").c_str())
        ("table,t", boost::program_options::value<boost::filesystem::path>(&gtw_table_file)->default_value("gtw_config.json"),
                    std::string("Specify config path. By default: 'gtw_config.json'").c_str());

    boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
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

    if (vm.count("debug"))
    {
        setDbgLevel(debug_level);
    }

    if (vm.count("sink"))
    {
        setDbgSink(debug_sink);
    }

    printDebug("MqttGateway/%s: HomeBrain core <-> MQTT Gateway Starting...", __FUNCTION__);

    try
    {
        std::unique_ptr<MqttGateway::CMqttGatewayImpl> core_gateway;
        core_gateway.reset(new MqttGateway::CMqttGatewayImpl(std::make_shared<Parsers::CConfigParser>(config_file.string()),
                                                             std::make_shared<Parsers::CGtwTableParser>(gtw_table_file.string())));

        core_gateway->performStart();

        // Setup signal handlers
        configureSignalHandlers();

        while(!isQuit())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        printDebug("MqttGateway/%s: HomeBrain core <-> MQTT Gateway Stopping...", __FUNCTION__);

        // close all activity
        core_gateway->performStop();

        printDebug("MqttGateway/%s: HomeBrain core <-> MQTT Gateway Stopped...", __FUNCTION__);
    }
    catch (const std::exception& e)
    {
        printDebug("MqttGateway/%s: Error description: %s", __FUNCTION__, e.what());
        printDebug("MqttGateway/%s: Gateway is closings", __FUNCTION__);
        return 1;
    }

    return EXIT_SUCCESS;
}
