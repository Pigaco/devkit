#include <piga/devkit/InstallRequest.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace piga
{
namespace devkit
{
InstallRequest::InstallRequest()
{
    
}
InstallRequest::~InstallRequest()
{
    
}
void InstallRequest::load_file(const std::string &filename)
{
    this->filename = boost::filesystem::path(filename).filename().string();
    std::ifstream opened_file(filename);
    file = std::string(std::istreambuf_iterator<char>(opened_file),
        std::istreambuf_iterator<char>());
    opened_file.close();
}
std::string InstallRequest::save_file(const std::string &path)
{
    std::string file_path = path + "" + boost::filesystem::path(filename).filename().string();
    std::ofstream opened_file(file_path);
    opened_file << file;
    
    opened_file.close();
    
    return file_path;
}

}
}
