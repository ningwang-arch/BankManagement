#include "WebUI.hpp"
#include <boost/program_options.hpp>

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 8080
#endif // !DEFAULT_PORT

std::string CONFIG_FILE = "config.json";

using namespace std;
namespace po = boost::program_options;

int main(int argc, char const *argv[])
{
    int port = DEFAULT_PORT;

    // Declare the supported options.

    po::options_description desc("options");
    desc.add_options()("help,h", "print help message")("config,c", po::value<string>()->default_value(CONFIG_FILE), "config file")("port,p", po::value<int>()->default_value(DEFAULT_PORT), "port number");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << endl;
        return 0;
    }

    if (vm.count("config"))
    {
        CONFIG_FILE = vm["config"].as<string>();
    }

    if (vm.count("port"))
    {
        port = vm["port"].as<int>();
    }

    srand((unsigned)time(NULL));
    WebUI w(port);
    w.init();
    w.start();
    return 0;
}
