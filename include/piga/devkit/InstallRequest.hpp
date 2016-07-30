#pragma once

#include <string>
#include <piga/devkit/Request.hpp>
#include <boost/serialization/string.hpp>

namespace piga
{
namespace devkit
{
class InstallRequest
{
public:
    InstallRequest();
    virtual ~InstallRequest();
    
    std::string file = "";
    std::string filename = "";
    
    void load_file(const std::string &filename);
    std::string save_file(const std::string &path);
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int v)
    {
        ar & BOOST_SERIALIZATION_NVP(filename);
        ar & BOOST_SERIALIZATION_NVP(file);
    }
};
}
}
