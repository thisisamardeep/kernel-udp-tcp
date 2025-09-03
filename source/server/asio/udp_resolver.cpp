
#include "asio/udp_resolver.h"

namespace AsioWrapper {
namespace Asio {

UDPResolver::UDPResolver(const std::shared_ptr<Service>& service)
    : _service(service),
      _io_service(_service->GetAsioService()),
      _strand(*_io_service),
      _strand_required(_service->IsStrandRequired()),
      _resolver(*_io_service)
{
    if (service == nullptr)
        throw AsioUtils::ArgumentException("Asio service is invalid!");
}

}
}
