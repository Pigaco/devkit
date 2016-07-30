#pragma once

#include <stdint.h>
#include <piga/devkit/Config.hpp>

#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/export.hpp>

namespace piga
{
namespace devkit
{
class Request
{
public:
    enum class Type {
        INSTALL,
        RESTART,
        REBOOT,
        STOP,
        
        _COUNT
    };
    
    static inline Request::Type getTypeFromChar(char c) {
        switch(c) {
            case 'I':
                return Type::INSTALL;
            case 'r':
                return Type::RESTART;
            case 'R':
                return Type::REBOOT;
            case 'S':
                return Type::STOP;
        }
        return Type::RESTART;
    }
    static inline char getCharFromType(Request::Type type) {
        switch(type) {
            case Type::INSTALL:
                return 'I';
            case Type::RESTART:
                return 'r';
            case Type::REBOOT:
                return 'R';
            case Type::STOP:
                return 'S';
        }
    }
    
    Request();
    virtual ~Request() {}
};
}
}
