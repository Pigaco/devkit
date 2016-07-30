#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/export.hpp>

#define PIGA_DEVKIT_SERVER_REQUEST_TERMINATOR "\n\n\n\n__REQUEST_FINISHED__"

namespace piga
{
namespace devkit
{
class Server
{
public:
    Server(uint32_t port, const std::string &password = "");
    virtual ~Server();
    
    int run();
    
    void stop();
    
    void set_running(bool running);
    bool is_running();
    
    void set_password(const std::string &password);
    const std::string& get_password();
    
    void install(boost::archive::xml_iarchive *iarchive);
    void restart(boost::archive::xml_iarchive *iarchive);
    void reboot(boost::archive::xml_iarchive *iarchive);
    void stop(boost::archive::xml_iarchive *iarchive);
    
    void handle_client(boost::asio::ip::tcp::socket *client_socket);
private:
    uint32_t m_port = 0;
    std::string m_password;
    bool m_running = false;
    
    boost::asio::io_service m_io_service;
};
}
}
