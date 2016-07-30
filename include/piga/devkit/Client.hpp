#pragma once

#include <string>
#include <piga/devkit/Request.hpp>

#include <boost/asio/io_service.hpp>

#include <functional>

namespace piga
{
namespace devkit
{
class Client
{
public:
    Client(const std::string &host, uint32_t port, const std::string &password = "");
    virtual ~Client();
    
    void set_password(const std::string &password);
    const std::string& get_password();
    
    void install(const std::string &filename);
    void restart();
    void reboot();
    void stop();
    
    void send_request(piga::devkit::Request::Type type, std::function<void(boost::archive::xml_oarchive*)> req);
private:
    uint32_t m_port;
    std::string m_host;
    std::string m_password;
    boost::asio::io_service m_io_service;
};
}
}
