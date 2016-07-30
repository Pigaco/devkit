#include <iostream>
#include <string>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include <boost/program_options.hpp>

#include <piga/devkit/Server.hpp>
#include <piga/devkit/Client.hpp>

#include <piga/devkit/InstallRequest.hpp>

BOOST_CLASS_EXPORT(piga::devkit::InstallRequest)

using std::cout;
using std::endl;

using namespace piga::devkit;

Server *global_server = nullptr;

// http://stackoverflow.com/a/1641223
void signal_handler(int s){
    cout << endl << "[Devkit] Cought SIGINT; Stoppig Server." << endl;
    
    if(global_server) {
        global_server->stop();
    }
}

namespace po = boost::program_options;
po::options_description desc("Options");

void print_help()
{
    cout << "Pigaco Devkit; Made by maximaximal." << endl;
    cout << desc << endl;
}

int main(int argc, char* argv[])
{
    // Setup SIGINT signal handling.
    struct sigaction sigIntHandler;
    
    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    
    sigaction(SIGINT, &sigIntHandler, NULL);
    
    bool action_taken = false;
    
    // Parse the parameters to the Devkit utility.
    desc.add_options()
        ("help", "Show a help message.")
        ("server", po::value<uint32_t>()->required(), "Run the devkit as server on the specified port (has to be run as root and on the host).")
        ("client", po::value<std::vector<std::string> >()->required()->multitoken(), "Run the devkit as client, which needs to have an address (host port).")
        ("install", po::value<std::string>()->required(), "[CLIENT-ONLY] Install the specified .deb package.")
        ("restart", "[CLIENT-ONLY] Restart the Pigaco system (no system reboot, only software)")
        ("reboot", "[CLIENT-ONLY] Reboot the Pigaco system (complete system reboot)")
        ("daemon", "[SERVER-ONLY] Restart the server each time an exception was thrown, this is save for daemon use.")
        ("password", po::value<std::string>(), "The password for the client/server to make a connection possible.");
        
    po::variables_map vm;
    
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch(std::exception &e) {
        cout << "Exception while parsing command line arguments: " << endl;
        cout << e.what() << endl;
        return 1;
    }
    if(vm.count("help")) {
        action_taken = true;
        print_help();
        return 0;
    }
    if(vm.count("server")) {
        action_taken = true;
        uint32_t port = vm["server"].as<uint32_t>();
        Server server(port);
        server.set_running(true);
        
        if(vm.count("password")) 
            server.set_password(vm["password"].as<std::string>());
        
        global_server = &server;
        
        if(vm.count("daemon")) {
            cout << "[Devkit] Starting server in daemon mode." << endl;
            while(server.is_running()) {
                server.run();
            }
        } else {
            return server.run();
        }
    }
    if(vm.count("client")) {
        if(action_taken) {
            cout << "[Devkit] An action has already been performed; Client will not start." << endl;
            return 0;
        }
        action_taken = true;
        
        std::vector<std::string> host_port = vm["client"].as<std::vector<std::string>>();
        std::string host = host_port[0];
        uint32_t port = std::stoi(host_port[1]);
        
        Client client(host, port);
        
        if(vm.count("password")) 
            client.set_password(vm["password"].as<std::string>());
        
        if(vm.count("install")) {
            client.install(vm["install"].as<std::string>());
        }
    }
    
    if(!action_taken)
        print_help();
    
    return 0;
}
