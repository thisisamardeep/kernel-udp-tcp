#ifndef ASIOWRAPPER_ASIO_UDP_RESOLVER_H
#define ASIOWRAPPER_ASIO_UDP_RESOLVER_H

#include "service.h"

namespace AsioWrapper {
    namespace Asio {

        class UDPResolver
        {
        public:
            UDPResolver(const std::shared_ptr<Service>& service);
            UDPResolver(const UDPResolver&) = delete;
            UDPResolver(UDPResolver&&) = delete;
            virtual ~UDPResolver() { Cancel(); }

            UDPResolver& operator=(const UDPResolver&) = delete;
            UDPResolver& operator=(UDPResolver&&) = delete;

            std::shared_ptr<Service>& service() noexcept { return _service; }
            std::shared_ptr<asio::io_service>& io_service() noexcept { return _io_service; }
            asio::io_service::strand& strand() noexcept { return _strand; }
            asio::ip::udp::resolver& resolver() noexcept { return _resolver; }

            virtual void Cancel() { _resolver.cancel(); }

        private:
            std::shared_ptr<Service> _service;
            std::shared_ptr<asio::io_service> _io_service;
            asio::io_service::strand _strand;
            bool _strand_required;
            asio::ip::udp::resolver _resolver;
        };

    }
}

#endif