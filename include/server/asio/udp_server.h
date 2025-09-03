#ifndef ASIOWRAPPER_ASIO_UDP_SERVER_H
#define ASIOWRAPPER_ASIO_UDP_SERVER_H

#include "uuid.h"
#include "service.h"

namespace AsioWrapper {
namespace Asio {

class UDPServer : public std::enable_shared_from_this<UDPServer>
{
public:
    UDPServer(const std::shared_ptr<Service>& service, int port, InternetProtocol protocol = InternetProtocol::IPv4);
    UDPServer(const std::shared_ptr<Service>& service, const std::string& address, int port);
    UDPServer(const std::shared_ptr<Service>& service, const asio::ip::udp::endpoint& endpoint);
    UDPServer(const UDPServer&) = delete;
    UDPServer(UDPServer&&) = delete;
    virtual ~UDPServer() = default;

    UDPServer& operator=(const UDPServer&) = delete;
    UDPServer& operator=(UDPServer&&) = delete;

    const AsioUtils::UUID& id() const noexcept { return _id; }

    std::shared_ptr<Service>& service() noexcept { return _service; }
    std::shared_ptr<asio::io_service>& io_service() noexcept { return _io_service; }
    asio::io_service::strand& strand() noexcept { return _strand; }
    asio::ip::udp::endpoint& endpoint() noexcept { return _endpoint; }
    asio::ip::udp::endpoint& multicast_endpoint() noexcept { return _multicast_endpoint; }

    const std::string& address() const noexcept { return _address; }
    int port() const noexcept { return _port; }

    uint64_t bytes_pending() const noexcept { return _bytes_sending; }
    uint64_t bytes_sent() const noexcept { return _bytes_sent; }
    uint64_t bytes_received() const noexcept { return _bytes_received; }
    uint64_t datagrams_sent() const noexcept { return _datagrams_sent; }
    uint64_t datagrams_received() const noexcept { return _datagrams_received; }

    bool option_reuse_address() const noexcept { return _option_reuse_address; }
    bool option_reuse_port() const noexcept { return _option_reuse_port; }
    size_t option_receive_buffer_limit() const noexcept { return _receive_buffer_limit; }
    size_t option_receive_buffer_size() const;
    size_t option_send_buffer_limit() const noexcept { return _send_buffer_limit; }
    size_t option_send_buffer_size() const;

    bool IsStarted() const noexcept { return _started; }

    virtual bool Start();
    virtual bool Start(const std::string& multicast_address, int multicast_port);
    virtual bool Start(const asio::ip::udp::endpoint& multicast_endpoint);
    virtual bool Stop();
    virtual bool Restart();

    virtual size_t Multicast(const void* buffer, size_t size);
    virtual size_t Multicast(std::string_view text) { return Multicast(text.data(), text.size()); }

    virtual size_t Multicast(const void* buffer, size_t size, const AsioUtils::Timespan& timeout);
    virtual size_t Multicast(std::string_view text, const AsioUtils::Timespan& timeout) { return Multicast(text.data(), text.size(), timeout); }

    virtual bool MulticastAsync(const void* buffer, size_t size);
    virtual bool MulticastAsync(std::string_view text) { return MulticastAsync(text.data(), text.size()); }

    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size);
    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, std::string_view text) { return Send(endpoint, text.data(), text.size()); }

    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size, const AsioUtils::Timespan& timeout);
    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, std::string_view text, const AsioUtils::Timespan& timeout) { return Send(endpoint, text.data(), text.size(), timeout); }

    virtual bool SendAsync(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size);
    virtual bool SendAsync(const asio::ip::udp::endpoint& endpoint, std::string_view text) { return SendAsync(endpoint, text.data(), text.size()); }

    virtual size_t Receive(asio::ip::udp::endpoint& endpoint, void* buffer, size_t size);
    virtual std::string Receive(asio::ip::udp::endpoint& endpoint, size_t size);

    virtual size_t Receive(asio::ip::udp::endpoint& endpoint, void* buffer, size_t size, const AsioUtils::Timespan& timeout);
    virtual std::string Receive(asio::ip::udp::endpoint& endpoint, size_t size, const AsioUtils::Timespan& timeout);

    virtual void ReceiveAsync();

    void SetupReuseAddress(bool enable) noexcept { _option_reuse_address = enable; }
    void SetupReusePort(bool enable) noexcept { _option_reuse_port = enable; }
    void SetupReceiveBufferLimit(size_t limit) noexcept { _receive_buffer_limit = limit; }
    void SetupReceiveBufferSize(size_t size);
    void SetupSendBufferLimit(size_t limit) noexcept { _send_buffer_limit = limit; }
    void SetupSendBufferSize(size_t size);

protected:
    virtual void onStarted() {}
    virtual void onStopped() {}

    virtual void onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size) {}
    virtual void onSent(const asio::ip::udp::endpoint& endpoint, size_t sent) {}

    virtual void onError(int error, const std::string& category, const std::string& message) {}

private:
    AsioUtils::UUID _id;
    std::shared_ptr<Service> _service;
    std::shared_ptr<asio::io_service> _io_service;
    asio::io_service::strand _strand;
    bool _strand_required;
    std::string _address;
    int _port;
    asio::ip::udp::endpoint _endpoint;
    asio::ip::udp::socket _socket;
    std::atomic<bool> _started;
    uint64_t _bytes_sending;
    uint64_t _bytes_sent;
    uint64_t _bytes_received;
    uint64_t _datagrams_sent;
    uint64_t _datagrams_received;
    asio::ip::udp::endpoint _multicast_endpoint;
    asio::ip::udp::endpoint _receive_endpoint;
    asio::ip::udp::endpoint _send_endpoint;
    bool _receiving;
    size_t _receive_buffer_limit{0};
    std::vector<uint8_t> _receive_buffer;
    HandlerStorage _receive_storage;
    bool _sending;
    size_t _send_buffer_limit{0};
    std::vector<uint8_t> _send_buffer;
    HandlerStorage _send_storage;
    bool _option_reuse_address;
    bool _option_reuse_port;

    void TryReceive();
    void ClearBuffers();
    void SendError(std::error_code ec);
};

}
}

#endif