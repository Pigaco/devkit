#include <iostream>
#include <piga/devkit/Server.hpp>
#include <piga/devkit/Request.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem/operations.hpp>

#include <piga/devkit/InstallRequest.hpp>

using std::cout;
using std::endl;

using boost::asio::ip::tcp;

namespace piga
{
namespace devkit
{
Server::Server(uint32_t port, const std::string &password)
    : m_port(port), m_password(password)
{
    
}
Server::~Server()
{
    
}

int Server::run()
{
    m_running = true;
    try {
        tcp::acceptor acceptor(m_io_service, tcp::endpoint(tcp::v4(), m_port));
        cout << "[Devkit] Devkit server started on port " << m_port << endl;
        while(m_running)
        {
            tcp::socket socket(m_io_service);
            acceptor.accept(socket);
            
            handle_client(&socket);
        }
    } catch(std::exception &e) {
        cout << "[Devkit] Cought an exception during server execution." << endl;
        cout << e.what() << endl;
    }
    cout << "[Devkit] Server stopped." << endl;
}
void Server::stop()
{
    m_running = false;
}
void Server::set_running(bool running)
{
    m_running = running;
}
bool Server::is_running()
{
    return m_running;
}
void Server::set_password(const std::string &password)
{
    m_password = password;
}
const std::string& Server::get_password()
{
    return m_password;
}
void Server::handle_client(tcp::socket *client_socket)
{
    cout << "[Devkit] Client connected from \"" << client_socket->remote_endpoint().address().to_string() << "\"" << endl;
    
    // Create dynamic buffers (should only be allocated while receiving data)
    boost::asio::streambuf buffer; 
    
    boost::system::error_code err; 
    boost::asio::read_until(*client_socket, buffer, PIGA_DEVKIT_SERVER_REQUEST_TERMINATOR, err);
    
    boost::asio::streambuf::const_buffers_type bufs = buffer.data();
    
    std::string received_data(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + buffer.size());
 
    // Erase the termination string.
    received_data.erase(received_data.length() - (strlen(PIGA_DEVKIT_SERVER_REQUEST_TERMINATOR)));
    
    char t = received_data[0];
    Request::Type type = Request::getTypeFromChar(t);
   
    // Erase the type char.
    received_data.erase(0, 1);
    
    std::istringstream ss(received_data);
    
    boost::archive::xml_iarchive iarchive(ss);
    
    uint32_t version_major = 0;
    uint32_t version_minor = 0;
    uint32_t version_patch = 0;
    uint32_t version_tweak = 0;
    
    std::string password;
    
    iarchive >> BOOST_SERIALIZATION_NVP(version_major);
    iarchive >> BOOST_SERIALIZATION_NVP(version_minor);
    iarchive >> BOOST_SERIALIZATION_NVP(version_patch);
    iarchive >> BOOST_SERIALIZATION_NVP(version_tweak);
    iarchive >> boost::serialization::make_nvp("password", password);
    
    if(password != m_password) {
        cout << "[Devkit] Passwords did not match." << endl;
        return;
    }
    
    switch(type) {
        case Request::Type::INSTALL:
            install(&iarchive);
            break;
        case Request::Type::REBOOT:
            break;
        case Request::Type::RESTART:
            break;
        case Request::Type::STOP:
            m_running = false;
            break;
    }
}
void Server::install(boost::archive::xml_iarchive *iarchive)
{
    cout << "[Devkit] Client issued an Install Request" << endl;
    InstallRequest req;
    
    (*iarchive) >> BOOST_SERIALIZATION_NVP(req);
    
    cout << "[Devkit] Saving temporary file to /tmp" << endl;
    std::string filename = req.save_file("/tmp/");
    
    cout << "[Devkit] Saving archive to ";
    cout << filename;
    cout << endl;
    
    cout << "[Devkit] Installing using \"sudo dpkg -i <package>\"" << endl;
    system(("sudo dpkg -i " + filename).c_str());
    
    cout << "[Devkit] Remove the file." << endl;
    boost::filesystem::remove(filename);
}
void Server::reboot(boost::archive::xml_iarchive *iarchive)
{

}
void Server::restart(boost::archive::xml_iarchive *iarchive)
{

}
void Server::stop(boost::archive::xml_iarchive *iarchive)
{

}
}
}
