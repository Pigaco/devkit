#include <piga/devkit/Client.hpp>

#include <piga/devkit/Request.hpp>
#include <piga/devkit/InstallRequest.hpp>

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <piga/devkit/Server.hpp>

using std::cout;
using std::endl;

namespace piga
{
namespace devkit
{
Client::Client(const std::string &host, uint32_t port, const std::string &password)
    : m_host(host), m_port(port), m_password(password)
{
    cout << "[Devkit] Client initialized with host \"" << host << ":" << port << "\"" << endl;
}
Client::~Client()
{
    
}

void Client::set_password(const std::string &password)
{
    m_password = password;
}
const std::string& Client::get_password()
{
    return m_password;
}

void Client::install(const std::string &filename)
{
    InstallRequest req;
    req.load_file(filename);
    
    send_request(Request::Type::INSTALL, [&req] (boost::archive::xml_oarchive *ar) {
        *ar << BOOST_SERIALIZATION_NVP(req);
    });
}
void Client::restart()
{
    
}
void Client::reboot()
{
    
}
void Client::stop()
{

}

void Client::send_request(Request::Type type, std::function<void(boost::archive::xml_oarchive*)> req)
{
    using boost::asio::ip::tcp;
    tcp::resolver resolver(m_io_service);
    tcp::resolver::query query(m_host, std::to_string(m_port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    
    tcp::socket socket(m_io_service);
    boost::asio::connect(socket, endpoint_iterator);
    
    boost::asio::streambuf buf;
    std::ostream os{&buf};
    
    char t = Request::getCharFromType(type);
    os << t;
    
    std::unique_ptr<boost::archive::xml_oarchive> ar 
        = std::unique_ptr<boost::archive::xml_oarchive>(new boost::archive::xml_oarchive(os));
    
    uint32_t version_major = PIGA_DEVKIT_VERSION_MAJOR;
    uint32_t version_minor = PIGA_DEVKIT_VERSION_MINOR;
    uint32_t version_patch = PIGA_DEVKIT_VERSION_PATCH;
    uint32_t version_tweak = PIGA_DEVKIT_VERSION_TWEAK;
    
    *ar << BOOST_SERIALIZATION_NVP(version_major);
    *ar << BOOST_SERIALIZATION_NVP(version_minor);
    *ar << BOOST_SERIALIZATION_NVP(version_patch);
    *ar << BOOST_SERIALIZATION_NVP(version_tweak);
    *ar << boost::serialization::make_nvp("password", m_password);
    
    req(ar.get());
    
    ar.reset();
    
    os << PIGA_DEVKIT_SERVER_REQUEST_TERMINATOR;
    
    boost::asio::write(socket, buf);
    
    cout << "[Devkit] Request sent." << endl;
}

}
}
