

#ifndef ASIOWRAPPER_ASIO_H
#define ASIOWRAPPER_ASIO_H

#include <iostream>
#pragma GCC system_header
#include <asio.hpp>


#define ASIO_STANDALONE
#define ASIO_SEPARATE_COMPILATION
#define ASIO_NO_WIN32_LEAN_AND_MEAN


namespace AsioWrapper {

namespace Asio {

enum class InternetProtocol
{
    IPv4,
    IPv6
};


template <class TOutputStream>
TOutputStream& operator<<(TOutputStream& stream, InternetProtocol protocol);

}
}



namespace AsioWrapper {
    namespace Asio {

        template <class TOutputStream>
        inline TOutputStream& operator<<(TOutputStream& stream, InternetProtocol protocol)
        {
            switch (protocol)
            {
                case InternetProtocol::IPv4:
                    stream << "IPv4";
                    break;
                case InternetProtocol::IPv6:
                    stream << "IPv6";
                    break;
                default:
                    stream << "<unknown>";
                    break;
            }
            return stream;
        }

    }
}

#endif
